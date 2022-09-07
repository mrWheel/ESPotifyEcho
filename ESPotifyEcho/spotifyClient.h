#pragma once
#include <Arduino.h>
//------ [WiFiClientSecure@2.0.0] (esp32 core 2.0.3)
#include <WiFiClientSecure.h>

struct HttpResult {
    int httpCode;
    String payload;
};

class spotifyClient 
{
  public:
    spotifyClient();
    
    void begin(const char* clientId, const char* clientSecret, const char* refreshToken, const char *newDeviceId);
    //spotifyClient(String clientId, String clientSecret, char *newDeviceId, String refreshToken);

    char   userId[25];
    char   deviceName[30];
    String playlistId;
    String currentUri;
    
    int fetchToken();
    //int ResumePlaying( String context_uri );
    int play(const char*, const char*, int);
    int play();
    int pause();
    int shuffle(bool);
    int repeat();
    int next();
    int previous();
    void setUserId(char *newUserId);
    int setDeviceId(char *newDeviceId);
    int setVolume(int volume_percent);
    int createPlaylist(String playlistName);
    int addToPlaylist(String trackUri);
    int deletePlaylistItems(const char*, const char*);
    String getDevices();
    String getPlaylists();
    String getPlaylists(const char*);
    String getPlaylistItems(const char*);
    String getUserId();
    //String GetAlbum();
    String getTracks(const char *type, String albumId, int limit, int offset);
    String getCurrentPlaying();
    String search(String type, String query );
    String getArtistAlbums(String artistId, int limit, int offset);
    String getPlaybackState();

private:
    WiFiClientSecure client;
    //String clientId;
    char clientId[38];        // 32+1?
    //String clientSecret;
    char   clientSecret[38];  // 32+1?
    String redirectUri;
    String accessToken;
    //String refreshToken;
    char   refreshToken[200];
    char   deviceId[60];
    //String deviceName;

    String parseAccessToken(String key, String json );
    HttpResult CallAPI( String method, String url, String body );

    // root certificate for spotify.com 
    const char* digicert_root_ca= \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
    "-----END CERTIFICATE-----\n";
};
