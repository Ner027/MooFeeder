#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <thread>
#include <QJsonDocument>
#include <QNetworkConfigurationManager>
#include "../inc/gui/cqmlinterface.h"
#include <iostream>
#include <memory>

std::string log = "wlan0     Scan completed :\n"
                  "          Cell 01 - Address: 9C:9D:7E:49:9D:03\n"
                  "                    Channel:44\n"
                  "                    Frequency:5.22 GHz (Channel 44)\n"
                  "                    Quality=61/70  Signal level=-49 dBm  \n"
                  "                    Encryption key:on\n"
                  "                    ESSID:\"MI_Network_5G\"\n"
                  "                    Bit Rates:6 Mb/s; 9 Mb/s; 12 Mb/s; 18 Mb/s; 24 Mb/s\n"
                  "                              36 Mb/s; 48 Mb/s; 54 Mb/s\n"
                  "                    Mode:Master\n"
                  "                    Extra:tsf=0000000000000000\n"
                  "                    Extra: Last beacon: 60ms ago\n"
                  "                    IE: Unknown: 000D4D495F4E6574776F726B5F3547\n"
                  "                    IE: Unknown: 01088C129824B048606C\n"
                  "                    IE: Unknown: 03012C\n"
                  "                    IE: Unknown: 200103\n"
                  "                    IE: Unknown: 23020F00\n"
                  "                    IE: Unknown: 2D1AEF0903FFFF000000000000000000000100000000000000000000\n"
                  "                    IE: Unknown: 3D162C050400000000000000000000000000000000000000\n"
                  "                    IE: Unknown: 7F080400000200000040\n"
                  "                    IE: Unknown: BF0CB2F98133FAFF0000FAFF0000\n"
                  "                    IE: Unknown: C005012A00FCFF\n"
                  "                    IE: Unknown: C304022E2E2E\n"
                  "                    IE: Unknown: FF1C230D01081A400004700C801DC18304010800FAFFFAFF391CC7711C07\n"
                  "                    IE: Unknown: FF0724F43F003DFCFF\n"
                  "                    IE: Unknown: FF0227A3\n"
                  "                    IE: Unknown: FF0E260008A9FF2FA9FF4575FF6575FF\n"
                  "                    IE: Unknown: DD180050F2020101800003A4000027A4000042435E0062322F00\n"
                  "                    IE: Unknown: DD0900037F01010000FF7F\n"
                  "                    IE: Unknown: DD168CFDF0040000494C51030209720100000000FDFF0000\n"
                  "                    IE: IEEE 802.11i/WPA2 Version 1\n"
                  "                        Group Cipher : TKIP\n"
                  "                        Pairwise Ciphers (2) : CCMP TKIP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: WPA Version 1\n"
                  "                        Group Cipher : TKIP\n"
                  "                        Pairwise Ciphers (2) : CCMP TKIP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: Unknown: DD7B0050F204104A0001101044000102103B00010310470010876543219ABCDEF012349C9D7E499D02102100067869616F6D69102300045241363710240004303030321042000531323334351054000800060050F20400011011000C5869616F4D69526F75746572100800020000103C0001021049000600372A000120\n"
                  "                    IE: Unknown: DD088CFDF00101020100\n"
                  "                    IE: Unknown: DD138CFDF001010201000201010303010100040101\n"
                  "          Cell 02 - Address: B0:BB:E5:85:DB:E4\n"
                  "                    Channel:6\n"
                  "                    Frequency:2.437 GHz (Channel 6)\n"
                  "                    Quality=32/70  Signal level=-78 dBm  \n"
                  "                    Encryption key:on\n"
                  "                    ESSID:\"NOS-DBE4\"\n"
                  "                    Bit Rates:1 Mb/s; 2 Mb/s; 5.5 Mb/s; 11 Mb/s; 18 Mb/s\n"
                  "                              24 Mb/s; 36 Mb/s; 54 Mb/s\n"
                  "                    Bit Rates:6 Mb/s; 9 Mb/s; 12 Mb/s; 48 Mb/s\n"
                  "                    Mode:Master\n"
                  "                    Extra:tsf=0000000000000000\n"
                  "                    Extra: Last beacon: 60ms ago\n"
                  "                    IE: Unknown: 00084E4F532D44424534\n"
                  "                    IE: Unknown: 010882848B962430486C\n"
                  "                    IE: Unknown: 030106\n"
                  "                    IE: Unknown: 23021000\n"
                  "                    IE: Unknown: 2A0104\n"
                  "                    IE: Unknown: 32040C121860\n"
                  "                    IE: IEEE 802.11i/WPA2 Version 1\n"
                  "                        Group Cipher : CCMP\n"
                  "                        Pairwise Ciphers (1) : CCMP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: Unknown: 0B050200080000\n"
                  "                    IE: Unknown: 46057000000000\n"
                  "                    IE: Unknown: 2D1AAD0917FFFFFF0000000000000000000000000000000000000000\n"
                  "                    IE: Unknown: 3D1606080400000000000000000000000000000000000000\n"
                  "                    IE: Unknown: 7F0A84000800000000C00140\n"
                  "                    IE: Unknown: FF1C23050018120010202002400040810000CC00EAFFEAFF1A1CC7711C07\n"
                  "                    IE: Unknown: FF072404000128FCFF\n"
                  "                    IE: Unknown: FF0E260C00A40820A408404308603208\n"
                  "                    IE: Unknown: DD9C0050F204104A0001101044000102103B000103104700106931FAC9DAB2B36C248B87D6AE33F9A610210008536167656D636F6D10230011536167656D636F6D5F464153543536373010240007312E302D312E301042000F4E37323230373737543030313633301054000800060050F204000110110016536167656D636F6D5F46415354353637302D4E3732321008000220081049000600372A000120\n"
                  "                    IE: Unknown: DD1A00904C0418BF0CB1018003EAFF0000EAFF0020C0050006000000\n"
                  "                    IE: Unknown: DD090010180202001C0000\n"
                  "                    IE: Unknown: DD180050F20201018C0003A4000027A4000042435E0062322F00\n"
                  "          Cell 03 - Address: 9C:9D:7E:49:9D:04\n"
                  "                    Channel:11\n"
                  "                    Frequency:2.462 GHz (Channel 11)\n"
                  "                    Quality=66/70  Signal level=-44 dBm  \n"
                  "                    Encryption key:on\n"
                  "                    ESSID:\"MI_Network\"\n"
                  "                    Bit Rates:1 Mb/s; 2 Mb/s; 5.5 Mb/s; 11 Mb/s; 6 Mb/s\n"
                  "                              9 Mb/s; 12 Mb/s; 18 Mb/s\n"
                  "                    Bit Rates:24 Mb/s; 36 Mb/s; 48 Mb/s; 54 Mb/s\n"
                  "                    Mode:Master\n"
                  "                    Extra:tsf=0000000000000000\n"
                  "                    Extra: Last beacon: 60ms ago\n"
                  "                    IE: Unknown: 000A4D495F4E6574776F726B\n"
                  "                    IE: Unknown: 010882848B960C121824\n"
                  "                    IE: Unknown: 03010B\n"
                  "                    IE: Unknown: 23020D00\n"
                  "                    IE: Unknown: 2A0100\n"
                  "                    IE: Unknown: 32043048606C\n"
                  "                    IE: Unknown: 2D1AEF1903FFFF000000000000000000000100000000000000000000\n"
                  "                    IE: Unknown: 3D160B070600000000000000000000000000000000000000\n"
                  "                    IE: Unknown: 7F080400000200000040\n"
                  "                    IE: Unknown: BF0C92398133FAFF0000FAFF0000\n"
                  "                    IE: Unknown: C005000900FCFF\n"
                  "                    IE: Unknown: FF1A230D01081A400002700C801DC18304010800FAFFFAFF191CC771\n"
                  "                    IE: Unknown: FF0724F43F0017FCFF\n"
                  "                    IE: Unknown: FF022723\n"
                  "                    IE: Unknown: FF0E260008A9FF2FA9FF4575FF6575FF\n"
                  "                    IE: Unknown: DD180050F2020101800003A4000027A4000042435E0062322F00\n"
                  "                    IE: Unknown: DD0900037F01010000FF7F\n"
                  "                    IE: Unknown: DD168CFDF0040000494C51030209720100000000FDFF0000\n"
                  "                    IE: IEEE 802.11i/WPA2 Version 1\n"
                  "                        Group Cipher : TKIP\n"
                  "                        Pairwise Ciphers (2) : CCMP TKIP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: WPA Version 1\n"
                  "                        Group Cipher : TKIP\n"
                  "                        Pairwise Ciphers (2) : CCMP TKIP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: Unknown: DD7B0050F204104A0001101044000102103B00010310470010876543219ABCDEF012349C9D7E499D02102100067869616F6D69102300045241363710240004303030321042000531323334351054000800060050F20400011011000C5869616F4D69526F75746572100800020000103C0001011049000600372A000120\n"
                  "                    IE: Unknown: DD088CFDF00101020100\n"
                  "                    IE: Unknown: DD138CFDF001010201000201010303010100040101\n"
                  "          Cell 04 - Address: 3E:CD:57:71:D1:C0\n"
                  "                    Channel:11\n"
                  "                    Frequency:2.462 GHz (Channel 11)\n"
                  "                    Quality=34/70  Signal level=-76 dBm  \n"
                  "                    Encryption key:on\n"
                  "                    ESSID:\"OpenWRT\"\n"
                  "                    Bit Rates:1 Mb/s; 2 Mb/s; 5.5 Mb/s; 11 Mb/s; 6 Mb/s\n"
                  "                              9 Mb/s; 12 Mb/s; 18 Mb/s\n"
                  "                    Bit Rates:24 Mb/s; 36 Mb/s; 48 Mb/s; 54 Mb/s\n"
                  "                    Mode:Master\n"
                  "                    Extra:tsf=0000000000000000\n"
                  "                    Extra: Last beacon: 60ms ago\n"
                  "                    IE: Unknown: 00074F70656E575254\n"
                  "                    IE: Unknown: 010882848B960C121824\n"
                  "                    IE: Unknown: 03010B\n"
                  "                    IE: Unknown: 2A0100\n"
                  "                    IE: Unknown: 32043048606C\n"
                  "                    IE: IEEE 802.11i/WPA2 Version 1\n"
                  "                        Group Cipher : CCMP\n"
                  "                        Pairwise Ciphers (1) : CCMP\n"
                  "                        Authentication Suites (1) : PSK\n"
                  "                    IE: Unknown: 0B050200000000\n"
                  "                    IE: Unknown: 3B025400\n"
                  "                    IE: Unknown: 2D1AEC0103FFFF000000000000000000000100000000000000000000\n"
                  "                    IE: Unknown: 3D160B070600000000000000000000000000000000000000\n"
                  "                    IE: Unknown: 7F080400000000000140\n"
                  "                    IE: Unknown: DD180050F2020101010003A4000027A4000042435E0062322F00";

std::string exec(const char* cmd)
{
    char buffer[128];
    uint8_t rdIdx = 0;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(&buffer[rdIdx], (128 - rdIdx), pipe.get()))
    {
        result += buffer;
    }

    std::cout << result << std::endl;

    return result;
}

std::vector<std::string> parseIwListMessage(std::string src)
{
    std::string line;
    std::vector<std::string> ret;
    size_t currentPosition = 0;

    currentPosition = src.find('\n');
    while (currentPosition != std::string::npos)
    {
        currentPosition = src.find('\n');

        line = src.substr(0, currentPosition);
        src = src.substr(currentPosition + 1);

        if (line.find("ESSID") == std::string::npos)
            continue;

        ret.push_back(line.substr(line.find(':') + 1));
    }

    return ret;
}

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine qmlEngine;
    QQuickView view(&qmlEngine, nullptr);

    auto ret = parseIwListMessage(log);

    for (const auto &item: ret)
    {
        std::cout << item << std::endl;
    }

    view.rootContext()->setContextProperty("QmlInterface", CQmlInterface::getInstance());
    view.setSource(QUrl("qrc:/qml/Main.qml"));
    view.show();

    return QGuiApplication::exec();
}
