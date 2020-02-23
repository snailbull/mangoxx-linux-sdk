/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
	char *out;
	cJSON *json;

	json = cJSON_Parse(text);
	if (!json)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	else
	{
		out = cJSON_Print(json);
		cJSON_Delete(json);
		printf("%s\n", out);
		free(out);
	}
}

/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f;
	long len;
	char *data;

	f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = (char *)malloc(len + 1);
	fread(data, 1, len, f);
	fclose(f);
	doit(data);
	free(data);
}

/* Used by some code below as an example datatype. */
struct record
{
	const char *precision;
	double lat, lon;
	const char *address, *city, *state, *zip, *country;
};

/* Create a bunch of objects as demonstration. */
void create_objects()
{
	cJSON *root, *fmt, *img, *thm, *fld;
	char *out;
	int i;	/* declare a few. */
	/* Our "days of the week" array: */
	const char *strings[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	/* Our matrix: */
	int numbers[3][3] = {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}};
	/* Our "gallery" item: */
	int ids[4] = {116, 943, 234, 38793};
	/* Our array of "records": */
	struct record fields[2] =
	{
		{"zip", 37.7668, -1.223959e+2, "", "SAN FRANCISCO", "CA", "94107", "US"},
		{"zip", 37.371991, -1.22026e+2, "", "SUNNYVALE", "CA", "94085", "US"}
	};

	/* Here we construct some JSON standards, from the JSON site. */

	/* Our "Video" datatype: */
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
	cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
	cJSON_AddStringToObject(fmt, "type",		"rect");
	cJSON_AddNumberToObject(fmt, "width",		1920);
	cJSON_AddNumberToObject(fmt, "height",		1080);
	cJSON_AddFalseToObject (fmt, "interlace");
	cJSON_AddNumberToObject(fmt, "frame rate",	24);

	out = cJSON_Print(root);
	cJSON_Delete(root);
	printf("%s\n", out);
	free(out);	/* Print to text, Delete the cJSON, print it, release the string. */

	/* Our "days of the week" array: */
	root = cJSON_CreateStringArray(strings, 7);

	out = cJSON_Print(root);
	cJSON_Delete(root);
	printf("%s\n", out);
	free(out);

	/* Our matrix: */
	root = cJSON_CreateArray();
	for (i = 0; i < 3; i++)
	{
		cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
	}

	/*	cJSON_ReplaceItemInArray(root,1,cJSON_CreateString("Replacement")); */

	out = cJSON_Print(root);
	cJSON_Delete(root);
	printf("%s\n", out);
	free(out);


	/* Our "gallery" item: */
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
	cJSON_AddNumberToObject(img, "Width", 800);
	cJSON_AddNumberToObject(img, "Height", 600);
	cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
	cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
	cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
	cJSON_AddNumberToObject(thm, "Height", 125);
	cJSON_AddStringToObject(thm, "Width", "100");
	cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));

	out = cJSON_Print(root);
	cJSON_Delete(root);
	printf("%s\n", out);
	free(out);

	/* Our array of "records": */

	root = cJSON_CreateArray();
	for (i = 0; i < 2; i++)
	{
		cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
		cJSON_AddStringToObject(fld, "precision", fields[i].precision);
		cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
		cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
		cJSON_AddStringToObject(fld, "Address", fields[i].address);
		cJSON_AddStringToObject(fld, "City", fields[i].city);
		cJSON_AddStringToObject(fld, "State", fields[i].state);
		cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
		cJSON_AddStringToObject(fld, "Country", fields[i].country);
	}

	/*	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); */

	out = cJSON_Print(root);
	cJSON_Delete(root);
	printf("%s\n", out);
	free(out);

}

/*
{
  "name": "Jack (\"Bee\") Nimble",
  "format": {
    "type": "rect",
    "width": 1920,
    "height": 1080,
    "interlace": false,
    "frame rate": 24
  }
}
*/
char text1[] =
	"{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";
	
/*
[
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
]
*/
char text2[] = "[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";

/*
[
  [
    0,
    -1,
    3
  ],
  [
    5,
    6,
    9
  ],
  [
    12,
    -96,
    78
  ]
]
*/
char text3[] = "[\n    [0, -1, 3],\n    [5, 6, 9],\n    [12, -96, 78]\n	]\n";

/*
{
  "Image": {
    "Width": 800,
    "Height": 600,
    "Title": "View from 15th Floor",
    "Thumbnail": {
      "Url": "http:/*www.example.com/image/481989943",
      "Height": 125,
      "Width": "100"
    },
    "IDs": [
      116,
      943,
      234,
      38793
    ]
  }
}
*/
char text4[] =
	"{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:/*www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}";

/*
[
  {
    "precision": "zip",
    "Latitude": 37.7668,
    "Longitude": -122.3959,
    "Address": "",
    "City": "SAN FRANCISCO",
    "State": "CA",
    "Zip": "94107",
    "Country": "US"
  },
  {
    "precision": "zip",
    "Latitude": 37.371991,
    "Longitude": -122.02602,
    "Address": "",
    "City": "SUNNYVALE",
    "State": "CA",
    "Zip": "94085",
    "Country": "US"
  }
]
*/	
char text5[] =
	"[\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.7668,\n	 \"Longitude\": -122.3959,\n	 \"Address\":   \"\",\n	 \"City\":      \"SAN FRANCISCO\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94107\",\n	 \"Country\":   \"US\"\n	 },\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.371991,\n	 \"Longitude\": -122.026020,\n	 \"Address\":   \"\",\n	 \"City\":      \"SUNNYVALE\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94085\",\n	 \"Country\":   \"US\"\n	 }\n	 ]";
char text6[] = 
	"{\"uart\":{\"common_config\":{\"baudrate\":9600,\"flowserver\":false},\"asr_config\":{\"network_tts\":true,\"local_tone\":true,\"asr_led\":true}}}";
int cJSON_test(void)
{
	/* Process each json textblock by parsing, then rebuilding: */
	doit(text1);
	doit(text2);
	doit(text3);
	doit(text4);
	doit(text5);

	/* Parse standard testfiles: */
	/*	dofile("../../tests/test1"); */
	/*	dofile("../../tests/test2"); */
	/*	dofile("../../tests/test3"); */
	/*	dofile("../../tests/test4"); */
	/*	dofile("../../tests/test5"); */

	/* Now some samplecode for building objects concisely: */
	create_objects();

	return 0;
}

/*******************************************************************************
 * json测试
 */
cJSON *json_test;
void json_test_init(int select)
{
	char *s;
	if (select==1)
		s = text1;
	else if (select==2)
		s = text2;
	else if (select==3)
		s = text3;
	else if (select==4)
		s = text4;
	else if (select==5)
		s = text5;
	else if (select==6)
		s = text6;
	
	json_test = cJSON_Parse(s);
	if (!json_test)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	else
	{
		printf("%08x\n", json_test);
	}
}
void json_test_exit(void)
{
	cJSON_Delete(json_test);
}
void json_test_print(void)
{
	char *out = cJSON_Print(json_test);
	printf("%s\n", out);
	free(out);
}
char *json_type[]=
{
"cJSON_False",
"cJSON_True",
"cJSON_NULL",
"cJSON_Number",
"cJSON_String",
"cJSON_Array",
"cJSON_Object",
};
void json_struct_print(void *p)
{
	cJSON *json = (cJSON*)p;
	printf("=========\n");
	printf("  next:%08x, prev:%08x, child:%08x\n",
		json->next, json->prev, json->child);
	printf("  %s\n", json_type[json->type]);
	printf("  %s\n", json->valuestring);
	printf("  %d\n", json->valueint);
	printf("  %lf\n", json->valuedouble);
	printf("  %s\n", json->string);
	printf("=========\n");
}
void tuya_json_test(void)
{
	char str[] = "{}";
	cJSON *root;
	root = cJSON_Parse(str);
	if (root == NULL)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
		return ;
	}
	cJSON *tmp = cJSON_GetObjectItem((cJSON *)root,"success");
	if(!tmp || (tmp && tmp->type != cJSON_True))
	{
		printf("HTTP_FUNC_REGISTER: device register false!\r\n");
		return ;
	}
	
	tmp = cJSON_GetObjectItem(root,"result");
	tmp = cJSON_GetObjectItem(tmp,"devId");
	printf("%s:%s\n", tmp->string, tmp->valuestring);
	
	cJSON_Delete(root);
}

void tuya_json_test2(char *dps_json)
{
	// dps cJSON
	cJSON *dps = cJSON_Parse(dps_json);
	if (dps == NULL)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
		return ;
	}
	// root cJSON
	cJSON *root = cJSON_CreateObject();
	if (NULL == root)
	{
		cJSON_Delete(dps);
		return ;
	}
	cJSON_AddStringToObject(root, "devId", "002debugsf000000sfV3");
	cJSON_AddItemToObject(root, "dps", dps);
	char *out = cJSON_PrintUnformatted(root);
	printf("out:%s", out);
	cJSON_Delete(root),	root = NULL;
	//cJSON_Delete(dps),	dps = NULL;
	free(out);
}

void json_array_test(int i)
{
	cJSON *root = cJSON_Parse(text4);
	if (root == NULL)
	{
		printf("err\r\n");
		return ;
	}
	char *out = cJSON_Print(root);
	printf("%s\r\n", out);
	free(out);
	
	cJSON *p = cJSON_GetObjectItem(root, "Image");
	if (p == NULL)
	{
		printf("Image error\r\n");
		return ;
	}
	
	cJSON *array = cJSON_GetObjectItem(p, "IDs");
	if ((array == NULL) || (array->type != cJSON_Array))
	{
		printf("array error!\r\n");
		cJSON_Delete(root);
		return ;
	}
	printf("array size=%d\r\n", cJSON_GetArraySize(array));
	
	cJSON *item = cJSON_GetArrayItem(array, i);
	if (item == NULL)
	{
		printf("item err\r\n");
		cJSON_Delete(root);
		return ;
	}
	printf("type:%d, valueint:%d\r\n", item->type, item->valueint);
	
	cJSON_Delete(root);
}

#define LOGD(fmt,...)	printf("[%s():%d] "fmt, __func__, __LINE__, ##__VA_ARGS__)

void speech_json_config(void)
{
	cJSON *common, *node;
	cJSON *root = cJSON_Parse(text6);
	if (root == NULL)
	{
		LOGD("json err!\n");
		return ;
	}

	common = cJSON_GetObjectItem(root, "uart");
	if (common == NULL) {
		LOGD("uart err\n");
	}

	node = cJSON_GetObjectItem(common, "common_config");
	if (node == NULL) {
		LOGD("common_config err\n");
	}
}
