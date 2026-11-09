/*
 * D-Five server entitlement check.
 *
 * This check is additive: it does not replace Rockstar/Cfx ownership or
 * platform checks. Operators must configure both the platform key required
 * by the upstream runtime and a D-Five key issued by api.diola.ir.
 */

#include "StdInc.h"

#include <CoreConsole.h>
#include <Error.h>
#include <GameServer.h>
#include <HttpClient.h>
#include <ServerInstanceBase.h>
#include <ServerInstanceBaseRef.h>
#include <json.hpp>

#include <cstdlib>
namespace
{
constexpr auto kLicenseTimeout = std::chrono::seconds(30);
constexpr const char* kLicenseEndpoint = "https://api.diola.ir/v1/licenses/validate";

HttpClient* g_httpClient;
bool g_licenseChecked = false;
bool g_licenseRequestSent = false;

[[noreturn]] void RefuseToStart(const std::string& reason)
{
	console::PrintError("D-Five Auth", "%s\n", reason);
	console::PrintError("D-Five Auth", "Refusing to start.\n");
	fflush(nullptr);
	std::_Exit(1);
}
}

static InitFunction dfiveAuthInit([]()
{
	g_httpClient = new HttpClient(L"DFiveServer/LicenseClient");

	fx::ServerInstanceBase::OnServerCreate.Connect([](fx::ServerInstanceBase* instance)
	{
		if (g_licenseChecked)
		{
			return;
		}

		auto variableManager = instance->GetComponent<console::Context>()->GetVariableManager();
		auto licenseVar = variableManager->FindEntryRaw("dfive_licenseKey");

		if (!licenseVar || licenseVar->GetValue().empty())
		{
			RefuseToStart("Set dfive_licenseKey in server.cfg.");
		}

		const std::string license = licenseVar->GetValue();
		instance->GetComponent<fx::GameServer>()->OnTick.Connect([license]()
		{
			if (g_licenseChecked || g_licenseRequestSent)
			{
				return;
			}

			g_licenseRequestSent = true;
			nlohmann::json requestBody = {
				{ "license", license },
				{ "product", "dfive-server" },
			};

			HttpRequestOptions options;
			options.ipv4 = true;
			options.addErrorBody = true;
			options.headers = { { "Content-Type", "application/json; charset=utf-8" } };
			options.timeoutNoResponse = std::chrono::duration_cast<std::chrono::milliseconds>(kLicenseTimeout);

			g_httpClient->DoPostRequest(kLicenseEndpoint, requestBody.dump(), options,
			[](bool success, const char* data, size_t length)
			{
				const std::string body = (data && length) ? std::string(data, length) : std::string{};
				nlohmann::json response;

				try
				{
					response = nlohmann::json::parse(body);
				}
				catch (const std::exception&)
				{
					RefuseToStart(success
						? "The D-Five licensing service returned malformed JSON."
						: "Could not connect to the D-Five licensing service.");
				}

				if (!success || !response.is_object() || !response.value("valid", false))
				{
					RefuseToStart(response.is_object()
						? response.value("message", "D-Five license validation failed.")
						: "D-Five license validation failed.");
				}

				console::Printf("D-Five Auth", "License accepted.\n");
				g_licenseChecked = true;
			});
		});
	}, 1);
});
