// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

/* This sample uses the _LL APIs of iothub_client for example purposes.
That does not mean that HTTP only works with the _LL APIs.
Simply changing the using the convenience layer (functions not having _LL)
and removing calls to _DoWork will yield the same results. */

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "iothubtransporthttp.h"
#include "iothubtransportmqtt.h"
#include "iothubtransportmqtt_websockets.h"
#include "iothub_client_options.h"
#include "serializer.h"
#include "serializer_devicetwin.h"
#include "client.h"

#ifdef _MSC_VER
extern int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ...);
#endif // _MSC_VER

#define SET_TRUSTED_CERT_IN_SAMPLES

#ifdef SET_TRUSTED_CERT_IN_SAMPLES
#include "certs.h"
#endif // SET_TRUSTED_CERT_IN_SAMPLES

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
char* connectionString = NULL;

bool g_use_proxy;
HTTP_PROXY_OPTIONS g_proxy_options;

static int callbackCounter;
static bool g_continueRunning;
static bool g_twinReport;
static char propText[1024];
#define MESSAGE_COUNT       5
#define DOWORK_LOOP_NUM     3

typedef struct EVENT_INSTANCE_TAG
{
	IOTHUB_MESSAGE_HANDLE messageHandle;
	size_t messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	int* counter = (int*)userContextCallback;
	const char* buffer;
	size_t size;
	MAP_HANDLE mapProperties;
	const char* messageId;
	const char* correlationId;
	const char* contentType;
	const char* contentEncoding;

	// Message properties
	if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
	{
		messageId = "<null>";
	}

	if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
	{
		correlationId = "<null>";
	}

	if ((contentType = IoTHubMessage_GetContentTypeSystemProperty(message)) == NULL)
	{
		contentType = "<null>";
	}

	if ((contentEncoding = IoTHubMessage_GetContentEncodingSystemProperty(message)) == NULL)
	{
		contentEncoding = "<null>";
	}

	// Message content
	if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		printf("unable to retrieve the message data\r\n");
	}
	else
	{
		(void)printf("Received Message [%d]\r\n Message ID: %s\r\n Correlation ID: %s\r\n Content-Type: %s\r\n Content-Encoding: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n",
			*counter, messageId, correlationId, contentType, contentEncoding, (int)size, buffer, (int)size);
		// If we receive the work 'quit' then we stop running
		if (size == (strlen("quit") * sizeof(char)) && memcmp(buffer, "quit", size) == 0)
		{
			g_continueRunning = false;
		}
	}

	// Retrieve properties from the message
	mapProperties = IoTHubMessage_Properties(message);
	if (mapProperties != NULL)
	{
		const char*const* keys;
		const char*const* values;
		size_t propertyCount = 0;
		if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK)
		{
			if (propertyCount > 0)
			{
				size_t index;

				printf(" Message Properties:\r\n");
				for (index = 0; index < propertyCount; index++)
				{
					(void)printf("\tKey: %s Value: %s\r\n", keys[index], values[index]);
				}
				(void)printf("\r\n");
			}
		}
	}

	/* Some device specific action code goes here... */
	(*counter)++;
	return IOTHUBMESSAGE_ACCEPTED;
}

// Define the Model
BEGIN_NAMESPACE(ConectedMouse);

DECLARE_MODEL(GrRoseMouse,
WITH_DATA(double, distance),
WITH_DATA(double, rotation),
WITH_DATA(int, clickCount),
WITH_METHOD(quit),
WITH_METHOD(turnLedOn),
WITH_METHOD(turnLedOff)
);

DECLARE_STRUCT(ThresholdD,
	double, value
);

DECLARE_STRUCT(ThresholdR,
	double, value,
	ascii_char_ptr, status
);

DECLARE_DEVICETWIN_MODEL(MouseState,
WITH_REPORTED_PROPERTY(ThresholdR, threshold)
);

DECLARE_DEVICETWIN_MODEL(MouseSettings,
WITH_DESIRED_PROPERTY(ThresholdD, threshold, onDesiredThreshold)
);

END_NAMESPACE(ConectedMouse);

void mouseReportedStateCallback(int status_code, void* userContextCallback)
{
	MouseState *mouse = (MouseState *)userContextCallback;

	printf("received states \033[43m%d\033[49m, reported threshold = %.1f\n", status_code, mouse->threshold.value);
}

void onDesiredThreshold(void* argument)
{
	// Note: The argument is NOT a pointer to threshold, but instead a pointer to the MODEL
	//       that contains threshold as one of its arguments.  In this case, it
	//       is MouseSettings*.

	MouseSettings *mouse = (MouseSettings *)argument;
	printf("received a new desired.threshold = \033[42m%.1f\033[49m\n", mouse->threshold.value);

	g_twinReport = true;
}

METHODRETURN_HANDLE quit(GrRoseMouse* device)
{
	(void)device;
	(void)printf("quit with Method.\r\n");

	g_continueRunning = false;

	METHODRETURN_HANDLE result = MethodReturn_Create(0, "{\"Message\":\"quit with Method\"}");
	return result;
}

METHODRETURN_HANDLE turnLedOn(GrRoseMouse* device)
{
	(void)device;
	(void)printf("\033[41mTurning LED on with Method.\033[49m\r\n");

	telemetry.ledOn = 1;

	METHODRETURN_HANDLE result = MethodReturn_Create(1, "{\"Message\":\"Turning fan on with Method\"}");
	return result;
}

METHODRETURN_HANDLE turnLedOff(GrRoseMouse* device)
{
	(void)device;
	(void)printf("\033[44mTurning LED off with Method.\033[49m\r\n");

	telemetry.ledOn = 0;

	METHODRETURN_HANDLE result = MethodReturn_Create(0, "{\"Message\":\"Turning fan off with Method\"}");
	return result;
}

static int ReceiveDeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
	int result;

	/*this is step  3: receive the method and push that payload into serializer (from below)*/
	char* payloadZeroTerminated = (char*)malloc(size + 1);
	if (payloadZeroTerminated == 0)
	{
		printf("failed to malloc\r\n");
		*resp_size = 0;
		*response = NULL;
		result = -1;
	}
	else
	{
		(void)memcpy(payloadZeroTerminated, payload, size);
		payloadZeroTerminated[size] = '\0';

		/*execute method - userContextCallback is of type deviceModel*/
		METHODRETURN_HANDLE methodResult = EXECUTE_METHOD(userContextCallback, method_name, payloadZeroTerminated);
		free(payloadZeroTerminated);

		if (methodResult == NULL)
		{
			printf("failed to EXECUTE_METHOD\r\n");
			*resp_size = 0;
			*response = NULL;
			result = -1;
		}
		else
		{
			/* get the serializer answer and push it in the networking stack*/
			const METHODRETURN_DATA* data = MethodReturn_GetReturn(methodResult);
			if (data == NULL)
			{
				printf("failed to MethodReturn_GetReturn\r\n");
				*resp_size = 0;
				*response = NULL;
				result = -1;
			}
			else
			{
				result = data->statusCode;
				if (data->jsonValue == NULL)
				{
					char* resp = "{}";
					*resp_size = strlen(resp);
					*response = (unsigned char*)malloc(*resp_size);
					(void)memcpy(*response, resp, *resp_size);
				}
				else
				{
					*resp_size = strlen(data->jsonValue);
					*response = (unsigned char*)malloc(*resp_size);
					(void)memcpy(*response, data->jsonValue, *resp_size);
				}
			}
			MethodReturn_Destroy(methodResult);
		}
	}
	return result;
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;

	(void)printf("Confirmation[%d] received for message tracking id = %zu with result = %s\r\n", callbackCounter, eventInstance->messageTrackingId, MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));

	/* Some device specific action code goes here... */
	callbackCounter++;
	IoTHubMessage_Destroy(eventInstance->messageHandle);
}

void iothub_client_run(int proto)
{
	IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

	EVENT_INSTANCE messages[MESSAGE_COUNT];
	int msg_id = 0;
	int receiveContext = 0;

	g_continueRunning = true;

	srand((unsigned int)time(NULL));

	callbackCounter = 0;

	if (platform_init() != 0)
	{
		(void)printf("Failed to initialize the platform.\r\n");
	}
	else {
		if (serializer_init(NULL) != SERIALIZER_OK)
		{
			(void)printf("Failed in serializer_init.");
		}
		else if (SERIALIZER_REGISTER_NAMESPACE(ConectedMouse) == NULL)
		{
			LogError("unable to SERIALIZER_REGISTER_NAMESPACE");
		}
		else
		{
			IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
			switch (proto) {
			case 0:
				(void)printf("Starting the IoTHub client sample HTTP...\r\n");
				protocol = HTTP_Protocol;
				break;
			case 1:
				(void)printf("Starting the IoTHub client sample MQTT...\r\n");
				protocol = MQTT_Protocol;
				break;
			case 2:
				(void)printf("Starting the IoTHub client sample MQTT over WebSocket...\r\n");
				protocol = MQTT_WebSocket_Protocol;
				break;
			default:
				platform_deinit();
				return;
			}

			if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol)) == NULL)
			{
				(void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
			}
			else
			{
				if (g_use_proxy)
				{
					if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_HTTP_PROXY, &g_proxy_options) != IOTHUB_CLIENT_OK)
					{
						printf("failure to set option \"HTTP Proxy\"\r\n");
					}
				}
#if 0
				long curl_verbose = 1;
				if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_CURL_VERBOSE, &curl_verbose) != IOTHUB_CLIENT_OK)
				{
					printf("failure to set option \"CURL Verbose\"\r\n");
				}

				unsigned int timeout = 241000;
				// Because it can poll "after 9 seconds" polls will happen effectively // at ~10 seconds.
				// Note that for scalabilty, the default value of minimumPollingTime
				// is 25 minutes. For more information, see:
				// https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
				unsigned int minimumPollingTime = 9;
				if (IoTHubClient_LL_SetOption(iotHubClientHandle, "timeout", &timeout) != IOTHUB_CLIENT_OK)
				{
					printf("failure to set option \"timeout\"\r\n");
				}

				if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
				{
					printf("failure to set option \"MinimumPollingTime\"\r\n");
				}
#endif
#if 1
				bool traceOn = 1;
				if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOn) != IOTHUB_CLIENT_OK)
				{
					printf("failure to set option \"log trace on\"\r\n");
				}
#endif
#ifdef SET_TRUSTED_CERT_IN_SAMPLES
				// For mbed add the certificate information
				if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_TRUSTED_CERT, certificates) != IOTHUB_CLIENT_OK)
				{
					printf("failure to set option \"TrustedCerts\"\r\n");
				}
#endif // SET_TRUSTED_CERT_IN_SAMPLES
				MouseState *mouseState = IoTHubDeviceTwin_LL_CreateMouseState(iotHubClientHandle);
				if (mouseState == NULL)
				{
					printf("Failure in IoTHubDeviceTwin_LL_CreateMouseState");
				}
				else
				{
					(void)printf("IoTHubDeviceTwin_LL_CreateMouseState...successful.\r\n");
				}
				MouseSettings *mouseSettings = IoTHubDeviceTwin_LL_CreateMouseSettings(iotHubClientHandle);
				if (mouseSettings == NULL)
				{
					printf("Failure in IoTHubDeviceTwin_LL_CreateMouseSettings");
				}
				else
				{
					(void)printf("IoTHubDeviceTwin_LL_CreateMouseSettings...successful.\r\n");
				}
				GrRoseMouse* myMouse = CREATE_MODEL_INSTANCE(ConectedMouse, GrRoseMouse);
				if (myMouse == NULL)
				{
					(void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
				}
				else if (IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, ReceiveDeviceMethodCallback, myMouse) != IOTHUB_CLIENT_OK)
				{
					(void)printf("ERROR: IoTHubClient_LL_SetDeviceMethodCallback..........FAILED!\r\n");
				}
				else
				{
					(void)printf("IoTHubClient_LL_SetDeviceMethodCallback...successful.\r\n");
				}
				/* Setting Message call back, so we can receive Commands. */
				if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK)
				{
					(void)printf("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
				}
				else
				{
					(void)printf("IoTHubClient_LL_SetMessageCallback...successful.\r\n");
				}

				/* Now that we are ready to receive commands, let's send some messages */
				int iterator = 4000;
				do
				{
					if (iterator >= 5000)
					{
						iterator = 0;
						int msgPos = msg_id % MESSAGE_COUNT;
						unsigned char *msgText;
						size_t msgSize;
						myMouse->distance = telemetry.distance;
						myMouse->rotation = telemetry.rotation;
						myMouse->clickCount = telemetry.clickCount;
						if (SERIALIZE(&msgText, &msgSize, myMouse->distance, myMouse->rotation, myMouse->clickCount) != CODEFIRST_OK)
						{
							(void)printf("Failed to serialize\r\n");
						}
						else if ((messages[msgPos].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, msgSize)) == NULL)
						{
							(void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
							free(msgText);
						}
						else
						{
							MAP_HANDLE propMap;

							messages[msgPos].messageTrackingId = msg_id;

							propMap = IoTHubMessage_Properties(messages[msgPos].messageHandle);
							(void)sprintf_s(propText, sizeof(propText), myMouse->rotation > mouseSettings->threshold.value ? "true" : "false");
							if (Map_AddOrUpdate(propMap, "rotationAlert", propText) != MAP_OK)
							{
								(void)printf("ERROR: Map_AddOrUpdate Failed!\r\n");
							}

							if (proto == 0) {
								(void)IoTHubMessage_SetContentTypeSystemProperty(messages[msgPos].messageHandle, "application/json");
								(void)IoTHubMessage_SetContentEncodingSystemProperty(messages[msgPos].messageHandle, "utf-8");
							}

							if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messages[msgPos].messageHandle, SendConfirmationCallback, &messages[msgPos]) != IOTHUB_CLIENT_OK)
							{
								(void)printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!\r\n");
							}
							else
							{
								(void)printf("IoTHubClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub.\r\n", msg_id);
							}

							free(msgText);
							msg_id++;
						}
					}
					else if (g_twinReport) {
						g_twinReport = false;
						mouseState->threshold.value = mouseSettings->threshold.value;
						mouseState->threshold.status = "success";
						IoTHubDeviceTwin_LL_SendReportedStateMouseState(mouseState, mouseReportedStateCallback, mouseState);
					}
					iterator++;

					IoTHubClient_LL_DoWork(iotHubClientHandle);
					ThreadAPI_Sleep(1);

				} while (g_continueRunning);

				(void)printf("iothub_client_run has gotten quit message, call DoWork %d more time to complete final sending...\r\n", DOWORK_LOOP_NUM);
				for (size_t index = 0; index < DOWORK_LOOP_NUM; index++)
				{
					IoTHubClient_LL_DoWork(iotHubClientHandle);
					ThreadAPI_Sleep(1);
				}

				if (mouseSettings != NULL)
					IoTHubDeviceTwin_LL_DestroyMouseSettings(mouseSettings);
				if (mouseState != NULL)
					IoTHubDeviceTwin_LL_DestroyMouseState(mouseState);
				if (myMouse != NULL)
					DESTROY_MODEL_INSTANCE(myMouse);
				IoTHubClient_LL_Destroy(iotHubClientHandle);
			}
			serializer_deinit();
		}
		platform_deinit();
	}
}

void iothub_client_init()
{
#if 0
	char *set_cs_argv[2] = {
		"set_cs",
		"[device connection string]"
	};
	set_cs_main(2, set_cs_argv);

	char *set_proxy_argv[2] = {
		"set_proxy",
		"example.com:8080"
	};
	set_proxy_main(2, set_proxy_argv);
#endif
}

int iothub_client_main(int argc, char **argv)
{
	if (connectionString == NULL) {
		printf("Not set connection string, use 'device csgen' or 'device setcs'.\n");
		return 0;
	}

	if (argc < 2) {
		iothub_client_run(1);
		return 0;
	}

	if (strcmp(argv[1], "http") == 0) {
		iothub_client_run(0);
		return 0;
	}
	else if (strcmp(argv[1], "mqtt") == 0) {
		iothub_client_run(1);
		return 0;
	}
	else if (strcmp(argv[1], "mqttows") == 0) {
		iothub_client_run(2);
		return 0;
	}

	printf("%s [http|mqtt|mqttows] \n", argv[0]);
	return 0;
}
