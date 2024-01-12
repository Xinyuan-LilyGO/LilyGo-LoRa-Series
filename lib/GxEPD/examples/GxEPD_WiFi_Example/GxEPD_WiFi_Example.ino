
// According to the board, cancel the corresponding macro definition
// #define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28
// #define LILYGO_T5_V102
// #define LILYGO_T5_V266
// #define LILYGO_EPD_DISPLAY_102      //Depend  https://github.com/adafruit/Adafruit_NeoPixel
// #define LILYGO_EPD_DISPLAY_154

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>

#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY_102)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#include <Adafruit_NeoPixel.h>             //Depend  https://github.com/adafruit/Adafruit_NeoPixel
#elif defined(LILYGO_T5_V266)
#include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
#elif defined(LILYGO_T5_V213)
#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
#else
// #include <GxDEPG0097BW/GxDEPG0097BW.h>     // 0.97" b/w  form DKE GROUP
// #include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51" b/w   form DKE GROUP
// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9" b/w/r  form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w    form DKE GROUP
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r  form GoodDisplay
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r form GoodDisplay
// #include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
// #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w Ultra wide temperature , form GoodDisplay
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w   form GoodDisplay
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r form GoodDisplay
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w   form GoodDisplay
// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5" b/w   form DKE GROUP
#endif

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


// #define USING_SOFT_SPI      //Uncomment this line to use software SPI

#if defined(USING_SOFT_SPI)
GxIO_Class io(EPD_SCLK, EPD_MISO, EPD_MOSI,  EPD_CS, EPD_DC,  EPD_RSET);
#else
GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
#endif
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);


#if defined (ESP8266)
#include <ESP8266WiFi.h>
#define USE_BearSSL true
#endif

#include <WiFiClient.h>
#include <WiFiClientSecure.h>

const char *ssid     = "........";
const char *password = "........";
const int httpPort  = 80;
const int httpsPort = 443;
const char *fp_api_github_com = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";
const char *fp_github_com     = "ca 06 f5 6b 25 8b 7a 0d 4f 2b 05 47 09 39 47 86 51 15 19 84";
#if USE_BearSSL
const char fp_rawcontent[20]  = {0xcc, 0xaa, 0x48, 0x48, 0x66, 0x46, 0x0e, 0x91, 0x53, 0x2c, 0x9c, 0x7c, 0x23, 0x2a, 0xb1, 0x74, 0x4d, 0x29, 0x9d, 0x33};
#else
const char *fp_rawcontent     = "cc aa 48 48 66 46 0e 91 53 2c 9c 7c 23 2a b1 74 4d 29 9d 33";
#endif
const char *host_rawcontent   = "raw.githubusercontent.com";
const char *path_rawcontent   = "/ZinggJM/GxEPD2/master/extras/bitmaps/";
const char *path_prenticedavid   = "/prenticedavid/MCUFRIEND_kbv/master/extras/bitmaps/";

void showBitmapFrom_HTTP(const char *host, const char *path, const char *filename, int16_t x, int16_t y, bool with_color = true);
void showBitmapFrom_HTTPS(const char *host, const char *path, const char *filename, const char *fingerprint, int16_t x, int16_t y, bool with_color = true);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("GxEPD_WiFi_Example");

#if !defined(USING_SOFT_SPI)
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
#endif

    display.init(115200);

#ifdef RE_INIT_NEEDED
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA); // switch off AP
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.disconnect();
#endif

    if (!WiFi.getAutoConnect() || ( WiFi.getMode() != WIFI_STA) || ((WiFi.SSID() != ssid) && String(ssid) != "........")) {
        Serial.println();
        Serial.print("WiFi.getAutoConnect()=");
        Serial.println(WiFi.getAutoConnect());
        Serial.print("WiFi.SSID()=");
        Serial.println(WiFi.SSID());
        WiFi.mode(WIFI_STA); // switch off AP
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
    }
    int ConnectTimeout = 30; // 15 seconds
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        Serial.print(WiFi.status());
        if (--ConnectTimeout <= 0) {
            Serial.println();
            Serial.println("WiFi connect timeout");
            return;
        }
    }
    Serial.println();
    Serial.println("WiFi connected");

    // Print the IP address
    Serial.println(WiFi.localIP());

    drawBitmaps_200x200();
    drawBitmaps_other();

    //drawBitmaps_test();
    //drawBitmapsBuffered_test();

    Serial.println("GxEPD_WiFi_Example done");
}

void loop(void)
{
}

void drawBitmaps_200x200()
{
    int16_t x = (display.width() - 200) / 2;
    int16_t y = (display.height() - 200) / 2;
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "logo200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "first200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "second200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "third200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "fourth200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "fifth200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "sixth200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "seventh200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_rawcontent, "eighth200x200.bmp", fp_rawcontent, x, y);
    delay(2000);
}

void drawBitmaps_other()
{
    int16_t w2 = display.width() / 2;
    int16_t h2 = display.height() / 2;
    showBitmapFrom_HTTP("www.squix.org", "/blog/wunderground/", "chanceflurries.bmp", w2 - 50, h2 - 50, false);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "betty_1.bmp", fp_rawcontent, w2 - 100, h2 - 160);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "betty_4.bmp", fp_rawcontent, w2 - 102, h2 - 126);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "marilyn_240x240x8.bmp", fp_rawcontent, w2 - 120, h2 - 120);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "miniwoof.bmp", fp_rawcontent, w2 - 60, h2 - 80);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "test.bmp", fp_rawcontent, w2 - 100, h2 - 100);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "tiger.bmp", fp_rawcontent, w2 - 160, h2 - 120);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "tiger_178x160x4.bmp", fp_rawcontent, w2 - 89, h2 - 80);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "tiger_240x317x4.bmp", fp_rawcontent, w2 - 120, h2 - 160);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "tiger_320x200x24.bmp", fp_rawcontent, w2 - 160, h2 - 100);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "tiger16T.bmp", fp_rawcontent, w2 - 160, h2 - 100);
    delay(2000);
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "woof.bmp", fp_rawcontent, w2 - 100, h2 - 100);
    delay(2000);
}

void drawBitmaps_test()
{
    int16_t w2 = display.width() / 2;
    int16_t h2 = display.height() / 2;
    showBitmapFrom_HTTPS(host_rawcontent, path_prenticedavid, "betty_4.bmp", fp_rawcontent, w2 - 102, h2 - 126);
    delay(2000);
}

static const uint16_t input_buffer_pixels = 640; // may affect performance

static const uint16_t max_palette_pixels = 256; // for depth <= 8

uint8_t input_buffer[3 * input_buffer_pixels]; // up to depth 24
uint8_t mono_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w

void drawBitmapFrom_HTTP_ToBuffer(const char *host, const char *path, const char *filename, int16_t x, int16_t y, bool with_color)
{
    WiFiClient client;
    bool connection_ok = false;
    bool valid = false; // valid format to be handled
    bool flip = true; // bitmap is stored bottom-to-top
    uint32_t startTime = millis();
    if ((x >= display.width()) || (y >= display.height())) return;
    display.fillScreen(GxEPD_WHITE);
    Serial.print("connecting to "); Serial.println(host);
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    Serial.print("requesting URL: ");
    Serial.println(String("http://") + host + path + filename);
    client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: GxEPD_WiFi_Example\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("request sent");
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (!connection_ok) {
            connection_ok = line.startsWith("HTTP/1.1 200 OK");
            if (connection_ok) Serial.println(line);
            //if (!connection_ok) Serial.println(line);
        }
        if (!connection_ok) Serial.println(line);
        //Serial.println(line);
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
    }
    if (!connection_ok) return;
    // Parse BMP header
    if (read16(client) == 0x4D42) { // BMP signature
        uint32_t fileSize = read32(client);
        uint32_t creatorBytes = read32(client);
        uint32_t imageOffset = read32(client); // Start of image data
        uint32_t headerSize = read32(client);
        uint32_t width  = read32(client);
        uint32_t height = read32(client);
        uint16_t planes = read16(client);
        uint16_t depth = read16(client); // bits per pixel
        uint32_t format = read32(client);
        uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
        if ((planes == 1) && ((format == 0) || (format == 3))) { // uncompressed is handled, 565 also
            Serial.print("File size: "); Serial.println(fileSize);
            Serial.print("Image Offset: "); Serial.println(imageOffset);
            Serial.print("Header size: "); Serial.println(headerSize);
            Serial.print("Bit Depth: "); Serial.println(depth);
            Serial.print("Image size: ");
            Serial.print(width);
            Serial.print('x');
            Serial.println(height);
            // BMP rows are padded (if needed) to 4-byte boundary
            uint32_t rowSize = (width * depth / 8 + 3) & ~3;
            if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
            if (height < 0) {
                height = -height;
                flip = false;
            }
            uint16_t w = width;
            uint16_t h = height;
            if ((x + w - 1) >= display.width())  w = display.width()  - x;
            if ((y + h - 1) >= display.height()) h = display.height() - y;
            valid = true;
            uint8_t bitmask = 0xFF;
            uint8_t bitshift = 8 - depth;
            uint16_t red, green, blue;
            bool whitish, colored;
            if (depth == 1) with_color = false;
            if (depth <= 8) {
                if (depth < 8) bitmask >>= depth;
                //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
                bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
                for (uint16_t pn = 0; pn < (1 << depth); pn++) {
                    blue  = client.read();
                    green = client.read();
                    red   = client.read();
                    client.read();
                    bytes_read += 4;
                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
                    mono_palette_buffer[pn / 8] |= whitish << pn % 8;
                    if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
                    color_palette_buffer[pn / 8] |= colored << pn % 8;
                    //Serial.print("0x00"); Serial.print(red, HEX); Serial.print(green, HEX); Serial.print(blue, HEX);
                    //Serial.print(" : "); Serial.print(whitish); Serial.print(", "); Serial.println(colored);
                }
            }
            display.fillScreen(GxEPD_WHITE);
            uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
            //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
            bytes_read += skip(client, rowPosition - bytes_read);
            for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) { // for each line
                if (!connection_ok || !(client.connected() || client.available())) break;
                delay(1); // yield() to avoid WDT
                uint32_t in_remain = rowSize;
                uint32_t in_idx = 0;
                uint32_t in_bytes = 0;
                uint8_t in_byte = 0; // for depth <= 8
                uint8_t in_bits = 0; // for depth <= 8
                uint16_t color = GxEPD_WHITE;
                for (uint16_t col = 0; col < w; col++) { // for each pixel
                    yield();
                    if (!connection_ok || !(client.connected() || client.available())) break;
                    // Time to read more pixel data?
                    if (in_idx >= in_bytes) { // ok, exact match for 24bit also (size IS multiple of 3)
                        uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
                        uint32_t got = read(client, input_buffer, get);
                        while ((got < get) && connection_ok) {
                            //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
                            uint32_t gotmore = read(client, input_buffer + got, get - got);
                            got += gotmore;
                            connection_ok = gotmore > 0;
                        }
                        in_bytes = got;
                        in_remain -= got;
                        bytes_read += got;
                    }
                    if (!connection_ok) {
                        Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
                        break;
                    }
                    switch (depth) {
                    case 24:
                        blue = input_buffer[in_idx++];
                        green = input_buffer[in_idx++];
                        red = input_buffer[in_idx++];
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                        break;
                    case 16: {
                        uint8_t lsb = input_buffer[in_idx++];
                        uint8_t msb = input_buffer[in_idx++];
                        if (format == 0) { // 555
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                            red   = (msb & 0x7C) << 1;
                        } else { // 565
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                            red   = (msb & 0xF8);
                        }
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    }
                    break;
                    case 1:
                    case 4:
                    case 8: {
                        if (0 == in_bits) {
                            in_byte = input_buffer[in_idx++];
                            in_bits = 8;
                        }
                        uint16_t pn = (in_byte >> bitshift) & bitmask;
                        whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        in_byte <<= depth;
                        in_bits -= depth;
                    }
                    break;
                    }
                    if (whitish) {
                        color = GxEPD_WHITE;
                    } else if (colored && with_color) {
                        color = GxEPD_RED;
                    } else {
                        color = GxEPD_BLACK;
                    }
                    uint16_t yrow = y + (flip ? h - row - 1 : row);
                    display.drawPixel(x + col, yrow, color);
                } // end pixel
            } // end line
        }
        Serial.print("bytes read "); Serial.println(bytes_read);
    }
    Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
    if (!valid) {
        Serial.println("bitmap format not handled.");
    }
}

void showBitmapFrom_HTTP(const char *host, const char *path, const char *filename, int16_t x, int16_t y, bool with_color)
{
    Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
    drawBitmapFrom_HTTP_ToBuffer(host, path, filename, x, y, with_color);
    display.update();
}

void drawBitmapFrom_HTTPS_ToBuffer(const char *host, const char *path, const char *filename, const char *fingerprint, int16_t x, int16_t y, bool with_color)
{
    // Use WiFiClientSecure class to create TLS connection
#if USE_BearSSL
    BearSSL::WiFiClientSecure client;
#else
    WiFiClientSecure client;
#endif
    bool connection_ok = false;
    bool valid = false; // valid format to be handled
    bool flip = true; // bitmap is stored bottom-to-top
    uint32_t startTime = millis();
    if ((x >= display.width()) || (y >= display.height())) return;
    display.fillScreen(GxEPD_WHITE);
    Serial.print("connecting to "); Serial.println(host);
#if USE_BearSSL
    if (fingerprint) client.setFingerprint((uint8_t *)fingerprint);
#endif
    if (!client.connect(host, httpsPort)) {
        Serial.println("connection failed");
        return;
    }
#if defined (ESP8266) && !USE_BearSSL
    if (fingerprint) {
        if (client.verify(fingerprint, host)) {
            Serial.println("certificate matches");
        } else {
            Serial.println("certificate doesn't match");
            return;
        }
    }
#endif
    Serial.print("requesting URL: ");
    Serial.println(String("https://") + host + path + filename);
    client.print(String("GET ") + path + filename + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: GxEPD_WiFi_Example\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("request sent");
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (!connection_ok) {
            connection_ok = line.startsWith("HTTP/1.1 200 OK");
            if (connection_ok) Serial.println(line);
            //if (!connection_ok) Serial.println(line);
        }
        if (!connection_ok) Serial.println(line);
        //Serial.println(line);
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
    }
    if (!connection_ok) return;
    // Parse BMP header
    if (read16(client) == 0x4D42) { // BMP signature
        uint32_t fileSize = read32(client);
        uint32_t creatorBytes = read32(client);
        uint32_t imageOffset = read32(client); // Start of image data
        uint32_t headerSize = read32(client);
        uint32_t width  = read32(client);
        uint32_t height = read32(client);
        uint16_t planes = read16(client);
        uint16_t depth = read16(client); // bits per pixel
        uint32_t format = read32(client);
        uint32_t bytes_read = 7 * 4 + 3 * 2; // read so far
        if ((planes == 1) && ((format == 0) || (format == 3))) { // uncompressed is handled, 565 also
            Serial.print("File size: "); Serial.println(fileSize);
            Serial.print("Image Offset: "); Serial.println(imageOffset);
            Serial.print("Header size: "); Serial.println(headerSize);
            Serial.print("Bit Depth: "); Serial.println(depth);
            Serial.print("Image size: ");
            Serial.print(width);
            Serial.print('x');
            Serial.println(height);
            // BMP rows are padded (if needed) to 4-byte boundary
            uint32_t rowSize = (width * depth / 8 + 3) & ~3;
            if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
            if (height < 0) {
                height = -height;
                flip = false;
            }
            uint16_t w = width;
            uint16_t h = height;
            if ((x + w - 1) >= display.width())  w = display.width()  - x;
            if ((y + h - 1) >= display.height()) h = display.height() - y;
            valid = true;
            uint8_t bitmask = 0xFF;
            uint8_t bitshift = 8 - depth;
            uint16_t red, green, blue;
            bool whitish, colored;
            if (depth == 1) with_color = false;
            if (depth <= 8) {
                if (depth < 8) bitmask >>= depth;
                //bytes_read += skip(client, 54 - bytes_read); //palette is always @ 54
                bytes_read += skip(client, imageOffset - (4 << depth) - bytes_read); // 54 for regular, diff for colorsimportant
                for (uint16_t pn = 0; pn < (1 << depth); pn++) {
                    blue  = client.read();
                    green = client.read();
                    red   = client.read();
                    client.read();
                    bytes_read += 4;
                    whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                    colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
                    mono_palette_buffer[pn / 8] |= whitish << pn % 8;
                    if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
                    color_palette_buffer[pn / 8] |= colored << pn % 8;
                    //Serial.print("0x00"); Serial.print(red, HEX); Serial.print(green, HEX); Serial.print(blue, HEX);
                    //Serial.print(" : "); Serial.print(whitish); Serial.print(", "); Serial.println(colored);
                }
            }
            display.fillScreen(GxEPD_WHITE);
            uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
            //Serial.print("skip "); Serial.println(rowPosition - bytes_read);
            bytes_read += skip(client, rowPosition - bytes_read);
            for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) { // for each line
                if (!connection_ok || !(client.connected() || client.available())) break;
                delay(1); // yield() to avoid WDT
                uint32_t in_remain = rowSize;
                uint32_t in_idx = 0;
                uint32_t in_bytes = 0;
                uint8_t in_byte = 0; // for depth <= 8
                uint8_t in_bits = 0; // for depth <= 8
                uint16_t color = GxEPD_WHITE;
                for (uint16_t col = 0; col < w; col++) { // for each pixel
                    yield();
                    if (!connection_ok || !(client.connected() || client.available())) break;
                    // Time to read more pixel data?
                    if (in_idx >= in_bytes) { // ok, exact match for 24bit also (size IS multiple of 3)
                        uint32_t get = in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain;
                        uint32_t got = read(client, input_buffer, get);
                        while ((got < get) && connection_ok) {
                            //Serial.print("got "); Serial.print(got); Serial.print(" < "); Serial.print(get); Serial.print(" @ "); Serial.println(bytes_read);
                            uint32_t gotmore = read(client, input_buffer + got, get - got);
                            got += gotmore;
                            connection_ok = gotmore > 0;
                        }
                        in_bytes = got;
                        in_remain -= got;
                        bytes_read += got;
                    }
                    if (!connection_ok) {
                        Serial.print("Error: got no more after "); Serial.print(bytes_read); Serial.println(" bytes read!");
                        break;
                    }
                    switch (depth) {
                    case 24:
                        blue = input_buffer[in_idx++];
                        green = input_buffer[in_idx++];
                        red = input_buffer[in_idx++];
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                        break;
                    case 16: {
                        uint8_t lsb = input_buffer[in_idx++];
                        uint8_t msb = input_buffer[in_idx++];
                        if (format == 0) { // 555
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                            red   = (msb & 0x7C) << 1;
                        } else { // 565
                            blue  = (lsb & 0x1F) << 3;
                            green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                            red   = (msb & 0xF8);
                        }
                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                    }
                    break;
                    case 1:
                    case 4:
                    case 8: {
                        if (0 == in_bits) {
                            in_byte = input_buffer[in_idx++];
                            in_bits = 8;
                        }
                        uint16_t pn = (in_byte >> bitshift) & bitmask;
                        whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                        in_byte <<= depth;
                        in_bits -= depth;
                    }
                    break;
                    }
                    if (whitish) {
                        color = GxEPD_WHITE;
                    } else if (colored && with_color) {
                        color = GxEPD_RED;
                    } else {
                        color = GxEPD_BLACK;
                    }
                    uint16_t yrow = y + (flip ? h - row - 1 : row);
                    display.drawPixel(x + col, yrow, color);
                } // end pixel
            } // end line
        }
        Serial.print("bytes read "); Serial.println(bytes_read);
    }
    Serial.print("loaded in "); Serial.print(millis() - startTime); Serial.println(" ms");
    if (!valid) {
        Serial.println("bitmap format not handled.");
    }
    client.stop();
}

void showBitmapFrom_HTTPS(const char *host, const char *path, const char *filename, const char *fingerprint, int16_t x, int16_t y, bool with_color)
{
    Serial.println(); Serial.print("downloading file \""); Serial.print(filename);  Serial.println("\"");
    drawBitmapFrom_HTTPS_ToBuffer(host, path, filename, fingerprint, x, y, with_color);
    display.update();
}

uint16_t read16(WiFiClient &client)
{
    // BMP data is stored little-endian, same as Arduino.
    uint16_t result;
    ((uint8_t *)&result)[0] = client.read(); // LSB
    ((uint8_t *)&result)[1] = client.read(); // MSB
    return result;
}

uint32_t read32(WiFiClient &client)
{
    // BMP data is stored little-endian, same as Arduino.
    uint32_t result;
    ((uint8_t *)&result)[0] = client.read(); // LSB
    ((uint8_t *)&result)[1] = client.read();
    ((uint8_t *)&result)[2] = client.read();
    ((uint8_t *)&result)[3] = client.read(); // MSB
    return result;
}

#if USE_BearSSL

uint32_t skip(BearSSL::WiFiClientSecure &client, int32_t bytes)
{
    int32_t remain = bytes;
    uint32_t start = millis();
    while (client.connected() && (remain > 0)) {
        if (client.available()) {
            int16_t v = client.read();
            remain--;
        } else delay(1);
        if (millis() - start > 2000) break; // don't hang forever
    }
    return bytes - remain;
}

uint32_t read(BearSSL::WiFiClientSecure &client, uint8_t *buffer, int32_t bytes)
{
    int32_t remain = bytes;
    uint32_t start = millis();
    while (client.connected() && (remain > 0)) {
        if (client.available()) {
            int16_t v = client.read();
            *buffer++ = uint8_t(v);
            remain--;
        } else delay(1);
        if (millis() - start > 2000) break; // don't hang forever
    }
    return bytes - remain;
}

#endif

uint32_t skip(WiFiClient &client, int32_t bytes)
{
    int32_t remain = bytes;
    uint32_t start = millis();
    while ((client.connected() || client.available()) && (remain > 0)) {
        if (client.available()) {
            int16_t v = client.read();
            remain--;
        } else delay(1);
        if (millis() - start > 2000) break; // don't hang forever
    }
    return bytes - remain;
}

uint32_t read(WiFiClient &client, uint8_t *buffer, int32_t bytes)
{
    int32_t remain = bytes;
    uint32_t start = millis();
    while ((client.connected() || client.available()) && (remain > 0)) {
        if (client.available()) {
            int16_t v = client.read();
            *buffer++ = uint8_t(v);
            remain--;
        } else delay(1);
        if (millis() - start > 2000) break; // don't hang forever
    }
    return bytes - remain;
}
