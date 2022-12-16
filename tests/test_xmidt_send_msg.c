/* You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <getopt.h>
#include <rbus.h>
#define TEST_VALUE_CHANGE   1
#define TEST_SUBSCRIBE      1
#define TEST_SUBSCRIBE_EX   1

static void asyncMethodHandler(
    rbusHandle_t handle, 
    char const* methodName, 
    rbusError_t error,
    rbusObject_t params)
{
    (void)handle;
    printf("asyncMethodHandler called: %s  error=%d\n", methodName, error);
    if(error == RBUS_ERROR_SUCCESS)
    {
	const char * msg_typeStr = NULL;
	printf("Inside Handler %p\n", params);
	if(params)
	{
		rbusValue_t msg_type = rbusObject_GetValue(params, "msg_type");
		if(msg_type)
		{
			if(rbusValue_GetType(msg_type) == RBUS_STRING)
			{
				msg_typeStr = rbusValue_GetString(msg_type, NULL);
				printf("msg_type value received is %s\n", msg_typeStr);
			}
		}
		else
		{
			printf("msg_type is empty\n");
		}
		rbusObject_fwrite(params, 1, stdout);
	//	rbusObject_Release(params);
	}
	else
	{
		printf("outParams empty\n");
	}
    }
    else
    {
	printf("ASYNC RESPONSE FAILURE, outParams\n");
        if(params !=NULL)
	{
		rbusObject_fwrite(params, 1, stdout);
		printf("outParams print done\n");
	}
	else
	{
		printf("outParams empty..\n");
	}
    }
}
int main(int argc, char *argv[])
{
    rbusHandle_t handle;
    rbusObject_t inParams;
    rbusValue_t value;
    int rc = RBUS_ERROR_SUCCESS;
    (void)(argc);
    (void)(argv);
    printf("constumer: start\n");
    rc = rbus_open(&handle, "EventConsumer");
    if(rc != RBUS_ERROR_SUCCESS)
    {
        printf("consumer: rbus_open failed: %d\n", rc);
        goto exit1;
    }

    int count = 0;

    rbusObject_Init(&inParams, NULL);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "telemetry2");
    rbusObject_SetValue(inParams, "source", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "event:/profile-notify/MyProfile1");
    rbusObject_SetValue(inParams, "dest", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "event");
    rbusObject_SetValue(inParams, "msg_type", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "application/json");
    rbusObject_SetValue(inParams, "content_type", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "{payload}");
    rbusObject_SetValue(inParams, "payload", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetInt32(value, 75);
    rbusObject_SetValue(inParams, "payloadlen", value);
    rbusValue_Release(value);

    rbusValue_Init(&value);
    rbusValue_SetString(value, "95");
    rbusObject_SetValue(inParams, "qos", value);
    rbusValue_Release(value);

    rc = rbusMethod_InvokeAsync(handle, "Device.X_RDK_Xmidt.SendData", inParams, asyncMethodHandler, 1900);
    printf("rc is %d\n", rc);
    rbusObject_Release(inParams);
    printf("consumer: rbusMethod_Invoke(Device.X_RDK_Xmidt.SendData) %s\n",
        rc == RBUS_ERROR_SUCCESS ? "success" : "fail");
	count++;
    sleep(2000);
    rbus_close(handle);
    while (1){};
exit1:
    printf("consumer: exit\n");
    return rc;
}
