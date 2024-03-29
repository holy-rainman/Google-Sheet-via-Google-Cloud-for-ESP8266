// Please follow the insructions on https://randomnerdtutorials.com/esp32-datalogging-google-sheets/

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "TMPL6jEr8VlQS"
#define BLYNK_TEMPLATE_NAME "buang"
#define BLYNK_AUTH_TOKEN    "gCNOaICiz5DfYya0mc2qUyLPPTRPM-SI"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "JHChua@unifi";
char pass[] = "adamdanial123";
BlynkTimer timer;

#include <ezTime.h>
Timezone local;

#include <Arduino.h>
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>
// Google Project ID
#define PROJECT_ID "testwokwi-414503"
// Service Account's client email
#define CLIENT_EMAIL "testwokwi@testwokwi-414503.iam.gserviceaccount.com"
// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCqfetEg42qTjgi\nOXkZMSzUWx2kLH73Iku8wBMDXiV4tqc58qF65Cf6IXJ+dmjlXC8cqrq+emh7XKV3\ny6bA5ggJgFlhUq1aJMBCI6x6ODJ+PQIapexzXYIa5E0g0lx+sBGCnkniMt34ijKq\nQapGn+bhLsYEf93zIF8yimhaLXXzNsA1gDBVL70TZCg4uj0QbBQUgd0OEmOCsrEJ\nmae5tHhJBu3wR34jeTm3yr4fut+tqKJrnlMQItnUu5kg6CsSQfOSl/M4zeNd0oGZ\nsrKEICNy5t24hwiQcQ9WWNemwy+dVPo2hT0pwgMImrVo44j2ppAR8Lr8oD023bAt\nXkNlQBoHAgMBAAECggEAOJtBQ4jIPvzkf1yEvBwLLeCoyUsqxqQg8GT3ix01pMxE\nXqWT/ukE6vQy/xESh+ChFcwL39Vlnlvm5oz76z0U+w6d3oXkX7MnEG6mxBenkCly\nauWfSbiIy6P0hcrCgh+0WKF0l4FbGLvN0aCmx/csYaS1g2XvC9u6L94rgpFogZ0n\nguco7n1Gtg4KuJz9yu5YpbOkHq+aZ/pDRmHo3VpxRbksUPhvBmPWVWQzNnES+jV4\nkYhS6A6Td66jM1Cv3c4mj0b7f/6NrWTEFqPpTVL+q4Wuofwx1xwdt+2/SHJ6lr62\ngOr8WEzJQ9OTw5QYY8xDrOydTm31X5FkAlSWjVHg/QKBgQDgdAnPGC+OXZDyqtEP\nR+o7HAmhd0Q+gbhomk4CH+qHrvbQR0hH/XQo6SfRvPWrF4HsFEONvEfTt6oDQVGE\nbpmoQ6t59/Vxf3gp7SRss5RWuY5ri0emLEw3Y6vSgdfoxwfgCiEPRJCxKoF6gDjp\nQCSS3hplmK+/sKVlV+O0wxr+SwKBgQDCdFFE6A4dn6Uj4yzOFnvdN5jdcqGW6U94\nXRHCojP7Y2pPw/cWfbStx1d8MX7H/hJlkxqxjtPk3lqwQcS+dSNzl6kCkAdunb2F\nfOtbnkVW3j+CIMpfN4mbUQ4OHNthvbDxWnCHh36t/GszltY8rTYtBl+752vyzBnK\nbCbSQnGNtQKBgCrEEUZjPlRq4X5LK7A0rJ18lx0rw1YBk2hi1jAzcpVNsfs3Sb9l\nvGc7HXop/ckOnpOjvMdZs3nUidE69dQ82owj8J05sItilTuv0nvfr4jirA8zp4Ay\n+oanOArQ8JmKqqZ2Z5woHUWUar0zhyRyM0XEidUHxZzJhIoNt/YuIouHAoGARjge\nyNVLk6L8zXLR0TIJVJ8pEn68n3RtADgLm89bGiKSV2+aoIKQZlif3Pc6oxm6pvdy\nIkBDgFhTQ2bclzJRtQJFQ+mi5W0M7VFo5FLJE096e5E/wxaZGH8ypiIBjtAjP2S7\nUhW5xzZpt912S77421V76KzvLGRLw+8+LZtUyikCgYEAuZMwt5Vr32KsE9EphxtR\nnMPdPsPwufYdM4AhNx0jzoO2rgn/nljSzkzhmSU02a78v/H8lUvDbipnM5tkAxur\nfy9/T7bvtABP4xPYo+LAdGWGWKx8BCkCE5A/lHVqEB+3qFQHfki1UmCD7j2NbzAl\ncSRwSZkbHwE8gY5iQSyv9zk=\n-----END PRIVATE KEY-----\n";
// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "1uw2-SINiyTh3d0-5kewrhqvque7_WiytMm61uEDkxKI";
void tokenStatusCallback(TokenInfo info);


int pb1,flag1=0,cnt1=0;
int pb2,flag2=0,cnt2=0;

BLYNK_CONNECTED() 
{ Blynk.sendInternal("utc", "time");      
  Blynk.sendInternal("utc", "tz_rule");
  Blynk.sendInternal("rtc", "sync");
  Blynk.syncAll();
}

BLYNK_WRITE(InternalPinUTC) 
{ String cmd = param[0].asStr();
  if (cmd == "time") 
  { const uint64_t utc_time = param[1].asLongLong();
    UTC.setTime(utc_time / 1000, utc_time % 1000);
  } 
  else if (cmd == "tz_rule") 
  { String tz_rule = param[1].asStr();
    local.setPosix(tz_rule);
  }
}

long ts;
BLYNK_WRITE(InternalPinRTC) { ts = param.asLong(); }
char tx2buf[100];

void localTime() 
{ sprintf(tx2buf,"Time:%d:%d:%d Day:%d/%d/%d",
  local.hour(),local.minute(),local.second(),
  local.day(),local.month(),local.year());
  
  Blynk.virtualWrite(V5,tx2buf);
  Serial.println(tx2buf);
}

void initGoogleSheet()
{ GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);
  GSheet.setTokenCallback(tokenStatusCallback);
  GSheet.setPrerefreshSeconds(10 * 60);
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}
void sendGoogleSheet()
{ bool ready = GSheet.ready();
  FirebaseJson response;

  Serial.println("\nAppend spreadsheet values...");
  Serial.println("----------------------------");
  FirebaseJson valueRange;

  char jam    = local.hour();
  char minit  = local.minute();
  
  valueRange.add("majorDimension", "COLUMNS");
  valueRange.set("values/[0]/[0]", (int)jam);
  valueRange.set("values/[1]/[0]", (int)minit);
  valueRange.set("values/[2]/[0]", cnt1);
  valueRange.set("values/[3]/[0]", cnt2);

  bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
  if (success)
  { response.toString(Serial, true);
    valueRange.clear();
  }
  else
  { Serial.println(GSheet.errorReason());
  }
  Serial.println();
  Serial.println(ESP.getFreeHeap());
}

void tokenStatusCallback(TokenInfo info)
{ if (info.status == token_status_error)
  { GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  }
  else
  { GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}

void cekInput()
{ pb1 = digitalRead(D1);
  pb2 = digitalRead(D2);
  
  if(pb1==0) { flag1=1; sendGoogleSheet(); cnt1++; }
  if(pb2==0) { flag2=1; sendGoogleSheet(); cnt2++; }
  Serial.print(pb1);
  Serial.println(pb2);
}

void setup()
{ Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L,localTime);
  timer.setInterval(10L,cekInput);

  pinMode(D1,INPUT_PULLUP);
  pinMode(D2,INPUT_PULLUP);

  initGoogleSheet();
}

void loop()
{ Blynk.run();
  timer.run();
}

