#ifndef _CJSON_TEST_H_
#define _CJSON_TEST_H_

int cJSON_test(void);
void json_test_init(int select);
void json_test_exit(void);
void json_struct_print(void *json);
void create_objects();
void tuya_json_test(void);
void tuya_json_test2(char *dps_json);
void json_array_test(int i);
void json_test_print(void);
void speech_json_config(void);

#endif
