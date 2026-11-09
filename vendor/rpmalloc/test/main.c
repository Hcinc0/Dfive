
#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#  define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#endif

#include <rpmalloc.h>
#include <thread.h>
#include <test.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define pointer_offset(ptr, ofs) (void*)((char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_diff(first, second) (ptrdiff_t)((const char*)(first) - (const char*)(second))

static size_t _hardware_threads;
static int _test_failed;

static void
test_initialize(void);

static int
test_fail_cb(const char* reason, const char* file, int line) {
	fprintf(stderr, "FAIL: %s @ %s:%d\n", reason, file, line);
	fflush(stderr);
	_test_failed = 1;
	return -1;
}

#define test_fail(msg) test_fail_cb(msg, __FILE__, __LINE__)

typedef struct _allocator_thread_arg {
	unsigned int        loops;
	unsigned int        passes; //max 4096
	unsigned int        datasize[32];
	unsigned int        num_datasize; //max 32
	int                 init_fini_each_loop;
	void**              pointers;
	void**              crossthread_pointers;
} allocator_thread_arg_t;

static void
heap_allocator_thread(void* argp) {
	allocator_thread_arg_t arg = *(allocator_thread_arg_t*)argp;
	unsigned int iloop = 0;
	unsigned int ipass = 0;
	unsigned int icheck = 0;
	unsigned int id = 0;
	void** addr;
	uint32_t* data;
	unsigned int cursize;
	unsigned int iwait = 0;
	int ret = 0;

	rpmalloc_heap_t* outer_heap = rpmalloc_heap_acquire();

	addr = rpmalloc_heap_alloc(outer_heap, sizeof(void*) * arg.passes);
	data = rpmalloc_heap_alloc(outer_heap, 512 * 1024);
	for (id = 0; id < 512 * 1024 / 4; ++id)
		data[id] = id;

	thread_sleep(1);

	for (iloop = 0; iloop < arg.loops; ++iloop) {
		rpmalloc_heap_t* heap = rpmalloc_heap_acquire();

		for (ipass = 0; ipass < arg.passes; ++ipass) {
			cursize = 4 + arg.datasize[(iloop + ipass + iwait) % arg.num_datasize] + ((iloop + ipass) % 1024);

			addr[ipass] = rpmalloc_heap_alloc(heap, 4 + cursize);
			if (addr[ipass] == 0) {
				ret = test_fail("Allocation failed");
				goto end;
			}

			*(uint32_t*)addr[ipass] = (uint32_t)cursize;
			memcpy(pointer_offset(addr[ipass], 4), data, cursize);

			for (icheck = 0; icheck < ipass; ++icheck) {
				if (addr[icheck] == addr[ipass]) {
					ret = test_fail("Identical pointer returned from allocation");
					goto end;
				}
				if (addr[icheck] < addr[ipass]) {
					if (pointer_offset(addr[icheck], *(uint32_t*)addr[icheck] + 4) > addr[ipass]) {
						ret = test_fail("Invalid pointer inside another block returned from allocation");
						goto end;
					}
				}
				else if (addr[icheck] > addr[ipass]) {
					if (pointer_offset(addr[ipass], *(uint32_t*)addr[ipass] + 4) > addr[icheck]) {
						ret = test_fail("Invalid pointer inside another block returned from allocation");
						goto end;
					}
				}
			}
		}

		for (ipass = 0; ipass < arg.passes; ++ipass) {
			cursize = *(uint32_t*)addr[ipass];

			if (memcmp(pointer_offset(addr[ipass], 4), data, cursize)) {
				ret = test_fail("Data corrupted");
				goto end;
			}
		}

		rpmalloc_heap_free_all(heap);
		rpmalloc_heap_release(heap);
	}

	rpmalloc_heap_free_all(outer_heap);
	rpmalloc_heap_release(outer_heap);

end:
	thread_exit((uintptr_t)ret);
}

static int
test_first_class_heaps(void) {
	uintptr_t thread[32];
	uintptr_t threadres[32];
	unsigned int i;
	size_t num_alloc_threads;
	allocator_thread_arg_t arg[32];

	rpmalloc_initialize();

	num_alloc_threads = _hardware_threads * 2;
	if (num_alloc_threads < 2)
		num_alloc_threads = 2;
	if (num_alloc_threads > 16)
		num_alloc_threads = 16;

	for (i = 0; i < num_alloc_threads; ++i) {
		arg[i].datasize[0] = 19;
		arg[i].datasize[1] = 249;
		arg[i].datasize[2] = 797;
		arg[i].datasize[3] = 3058;
		arg[i].datasize[4] = 47892;
		arg[i].datasize[5] = 173902;
		arg[i].datasize[6] = 389;
		arg[i].datasize[7] = 19;
		arg[i].datasize[8] = 2493;
		arg[i].datasize[9] = 7979;
		arg[i].datasize[10] = 3;
		arg[i].datasize[11] = 79374;
		arg[i].datasize[12] = 3432;
		arg[i].datasize[13] = 548;
		arg[i].datasize[14] = 38934;
		arg[i].datasize[15] = 234;
		arg[i].num_datasize = 16;
#if defined(__LLP64__) || defined(__LP64__) || defined(_WIN64)
		arg[i].loops = 100;
		arg[i].passes = 4000;
#else
		arg[i].loops = 50;
		arg[i].passes = 1000;
#endif
		arg[i].init_fini_each_loop = 1;

		thread_arg targ;
		targ.fn = heap_allocator_thread;
		targ.arg = &arg[i];

		thread[i] = thread_run(&targ);
	}

	thread_sleep(1000);

	for (i = 0; i < num_alloc_threads; ++i)
		threadres[i] = thread_join(thread[i]);

	rpmalloc_finalize();

	for (i = 0; i < num_alloc_threads; ++i) {
		if (threadres[i])
			return -1;
	}

	printf("First class heap tests passed\n");
	return 0;
}

static int got_error;

int
test_run(int argc, char** argv) {
	(void)sizeof(argc);
	(void)sizeof(argv);
	test_initialize();
	if (test_first_class_heaps())
		return -1;
	printf("All tests passed\n");
	return 0;
}

#if (defined(__APPLE__) && __APPLE__)
#  include <TargetConditionals.h>
#  if defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
#    define NO_MAIN 1
#  endif
#elif (defined(__linux__) || defined(__linux))
#  include <sched.h>
#endif

#if !defined(NO_MAIN)

int
main(int argc, char** argv) {
	return test_run(argc, argv);
}

#endif

#ifdef _WIN32
#include <windows.h>

static void
test_initialize(void) {
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	_hardware_threads = (size_t)system_info.dwNumberOfProcessors;
}

#elif (defined(__linux__) || defined(__linux))

static void
test_initialize(void) {
	cpu_set_t prevmask, testmask;
	CPU_ZERO(&prevmask);
	CPU_ZERO(&testmask);
	sched_getaffinity(0, sizeof(prevmask), &prevmask);     //Get current mask
	sched_setaffinity(0, sizeof(testmask), &testmask);     //Set zero mask
	sched_getaffinity(0, sizeof(testmask), &testmask);     //Get mask for all CPUs
	sched_setaffinity(0, sizeof(prevmask), &prevmask);     //Reset current mask
	int num = CPU_COUNT(&testmask);
	_hardware_threads = (size_t)(num > 1 ? num : 1);
}

#else

static void
test_initialize(void) {
	_hardware_threads = 1;
}

#endif
