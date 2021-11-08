#ifndef CLIENT_H
#define CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

struct telemetry_t
{
	int ledOn;
	float distance;
	float rotation;
	int clickCount;
};

extern struct telemetry_t telemetry;

void iothub_client_init(void);
int iothub_client_main(int argc, char **argv);
int set_cs_main(int argc, char **argv);
int set_proxy_main(int argc, char **argv);
int clear_proxy_main(int argc, char **argv);
int dps_csgen_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_H */
