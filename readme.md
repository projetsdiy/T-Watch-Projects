# LilyGo TTGO T-Watch projects list
The T-Watch series concentrates in a mini 20mm thick case an ESP32 development board powered by LiPo battery and a color touch screen (or not). The box accommodates a specialized expansion card (Lora, GPS, GPRS, etc.).
The T-Watch 2020 is a real connected watch with a design reminiscent of the Apple Watch. As this is primarily an ESP32 development board, it can be fully programmed with Arduino code!

1. T-Watch Hello World, [create first ESP32 project with the LilyGoWatch library (Arduino IDE or PlatformIO)](https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio)
2. T-Watch Get Started TFT display [Getting started with the ESP32 TFT_eSPI library for TTGO T-Watch units](https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi)
3. T-Watch screen orientation reference [Change screen orientation with reference](https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/#changeorientation)
4. [How to convert bitmap to XBM format and display image on TTGO T-Watch ith TFT_eSPI library](https://diyprojects.io/ttgo-t-watch-display-xbm-tft_espi-lvgl-images-esp32-arduino/)
5. [ How to convert in C++ color image with LVGL and display on TTGO T-Watch](https://diyprojects.io/ttgo-t-watch-display-xbm-tft_espi-lvgl-images-esp32-arduino/)
6. Switch between LVGL and TFT_eSPI screen on a TTGO T-Watch [Landing page is created with LVGL library](https://diyprojects.io/ttgo-t-watch-mix-lvgl-tft_espi-libraries-same-esp32-project/)
7. Switch between LVGL and TFT_eSPI screen on a TTGO T-Watch [Landing page is created with TFT_eSPI library](https://diyprojects.io/ttgo-t-watch-mix-lvgl-tft_espi-libraries-same-esp32-project/)
8. [T-Watch. Menu, pages, navigation between screens with TFT_eSPI](https://diyprojects.io/ttgo-t-watch-menu-pages-navigation-between-screens-tft_espi/)
9. [Draw Mandelbrot or Julia fractals (LVGL + TFT_eSPI)](https://diyprojects.io/ttgo-t-watch-draw-mandelbrot-julia-fractals-esp32-lvgl-tft_espi/)
10. [T-Watch. Sleep and wake-up ESP32 with BMA423 accelerometer or AXP202 button](https://diyprojects.io/t-watch-sleep-and-wake-up-with-bma423-accelerometer-or-esp32-button/)

## Get started with T-Watch
* [Which T-Watch to choose?](https://diyprojects.io/ttgo-t-watch-esp32-which-model-choose-available-expansion-boards/)
* [T-Watch and T-Block pinout](https://diyprojects.io/t-watch-t-block-esp32-core-pcb-expansion-boards-pinout/)
* [T-Watch 2020 pinout](https://diyprojects.io/t-watch-2020-esp32-ttgo-smart-watch-pinout-specifications/)
* [Get started with LiLyGoWatch library](https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio/) with Arduino IDE and PlatformIO
* [Model constants](https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio/#constantestwatch)
* [Expansion boards and peripheral constants]("https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio/#constantesaccessoirescartesextension)
* [Platformio.ini example](https://diyprojects.io/lilygowatch-esp32-ttgo-t-watch-get-started-ide-arduino-platformio/#platformioinitwach)
* [Buy a T-Watch](https://diyprojects.io/esp32-lilygo-ttgo-t-watch-connected-watches-to-program-yourself/)
* [Buy T-Watch expansion board](https://diyprojects.io/esp32-lilygo-ttgo-t-watch-connected-watches-to-program-yourself/#carteextensiontwatch) for T-Watch Touch, T-Watch-N or T-Block

## Develop a graphical interface with TFT_eSPI or LVGL
* [TFT_eSPI. Getting started with the library. Display text, shapes, touch detection](https://diyprojects.io/ttgo-t-watch-get-started-esp32-tft_espi-library/)
* [How to display XBM (TFT_eSPI) and C ++ (LVGL) images](https://diyprojects.io/ttgo-t-watch-display-xbm-tft_espi-lvgl-images-esp32-arduino/)
* [Mix the LVGL and TFT_eSPI libraries in the same ESP32 project](https://diyprojects.io/ttgo-t-watch-mix-lvgl-tft_espi-libraries-same-esp32-project/)

## T-Watch projets and games
* [Draw Mandelbrot or Julia fractals (LVGL + TFT_eSPI)](https://diyprojects.io/ttgo-t-watch-draw-mandelbrot-julia-fractals-esp32-lvgl-tft_espi/)

## Usefull links
* [LilyGo official website](http://www.lilygo.cn/)
* [Official LiLyGoWatch library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)
* [LVGL](https://github.com/lvgl)
* [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
* [AXP202, power controller datasheet](http://dl.linux-sunxi.org/AXP/AXP202%20Datasheet_v1.0_en.pdf)
* [NXP PCF8563 RTC clock](https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf)
* [Bosch Sensortec BMA423, 3-axis accelerometer](https://www.bosch-sensortec.com/products/motion-sensors/accelerometers/bma423.html)
* [BMA423 library developed by Bosch](https://github.com/BoschSensortec/BMA423-Sensor-API). [Adaptation for T-Watch](https://github.com/lewisxhe/BMA423_Library)
* [Seeed Studio Air 530 GPS](https://media.digikey.com/pdf/Data%20Sheets/Seeed%20Technology/109020022_Web.pdf) 
* [SIM800L, GRPS modem](https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf) 
* [SIM868, GPRS modem](https://simcom.ee/modules/gsm-gprs-gnss/sim868/) 
* [AcSIP S76GXB Lora + GPS](https://techship.com/products/acsip-s76gxb-developer-kit/) 

## Learn PlatformIO
It is easier and faster to develop Arduino code using PlatformIO. If you are new to programming, you can start by reading these articles
* [Install PIO on VSCode on Windows, macOS or Linux](https://diyprojects.io/install-ide-platformio-extension-visual-studio-code-vscode-windows-32-bit-linux/)
* [Platformio.ini, tips and useful settings. data_dir, monitor_speed, upload_port ...](https://diyprojects.io/platformio-ini-tips-and-useful-settings-data_dir-monitor_speed-upload_port/)
* [Common problems and solutions](https://diyprojects.io/develop-iot-project-arduino-platformio-problems-solutions/)
* [Migrate Arduino code to a PlatformIO project](https://diyprojects.io/migrate-project-arduino-ide-platformio-esp32-esp8266-esp01/)

_____

# Liste des projets T-Watch
La série T-Watch concentre dans un mini boîtier de 20 mm d'épaisseur une carte de développement ESP32 alimentée par batterie LiPo et un écran couleur tactile (ou non). Le boitier accueille une carte d'extension spécialisée (Lora, GPS, GPRS…). 
La T-Watch 2020 est une véritable montre connectée au design rappelant l'Apple Watch. Comme il s'agit avant tout d'une carte de développement ESP32, elle peut être entièrement programmée avec du code Arduino!

1. T-Watch Hello World, [créer votre 1er projet ESP32 avec la librairie LilyGoWatch sur l'IDE Arduino ou PlatformIO](https://projetsdiy.fr/lilygowatch-ttgo-t-watch-projet-esp32-ide-arduino-platformio/)
2. T-Watch Get Started TFT display [Débuter avec la librairie ESP32 TFT_eSPI pour les boitier TTGO T-Watch](https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi)
3. T-Watch screen orientation reference [Orientation de l'écran](https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/#changeorientation)
4. [Comment convertir un bitmap au format XBM et afficher l'image sur TTGO T-Watch avec la bibliothèque TFT_eSPI](https://projetsdiy.fr/ttgo-t-watch-afficher-images-xbm-tft_espi-lvgl-esp32-arduino/)
5. [Comment convertir une image couleur C ++ avec LVGL et afficher sur TTGO T-Watch](https://projetsdiy.fr/ttgo-t-watch-afficher-images-xbm-tft_espi-lvgl-esp32-arduino/)
6. Naviguer entre un écran LVGL et TFT_eSPI sur un TTGO T-Watch [La page principale est créée avec la bibliothèque LVGL](https://projetsdiy.fr/ttgo-t-watch-mixer-librairies-lvgl-tft_espi-projet-esp32/)
7. Naviguer entre l'écran LVGL et TFT_eSPI sur un TTGO T-Watch [La page principale est créée avec la bibliothèque TFT_eSPI](https://projetsdiy.fr/ttgo-t-watch-mixer-librairies-lvgl-tft_espi-projet-esp32/)
8. [T-Watch. Menu, pages, navigation entre écrans avec TFT_eSPI](https://projetsdiy.fr/ttgo-t-watch-menu-pages-navigation-ecrans-tft_espi/)
9. [Dessiner des fractales de Mandelbrot ou Julia](https://projetsdiy.fr/ttgo-t-watch-projet-fractales-mandelbrot-julia-esp32/)
10. [Mise en veille et réveil de l'ESP32 avec accéléromètre BMA423 ou AXP202](https://projetsdiy.fr/ttgo-t-watch-veille-deep-sleep-reveil-accelerometre-bma423-axp202-esp32/)
11. []()

## Débuter avec la T-Watch 
* [Quelle T-Watch choisir ?](https://projetsdiy.fr/ttgo-t-watch-esp32-quel-modele-choisir-cartes-dextension-disponibles/)
* [Repérage des broches de la T-Watch et T-Block](https://projetsdiy.fr/t-watch-tblock-reperage-broches-core-pcb-cartes-extension/)
* [Repérage des broches de la T-Watch 2020](https://projetsdiy.fr/t-watch-2020-reperage-des-broches-et-specifications-de-la-montre-connectee-esp32-ttgo/)
* [Débuter avec la librairie LiLyGoWatch (IDE Arduino et PlatformIO)](https://projetsdiy.fr/lilygowatch-ttgo-t-watch-projet-esp32-ide-arduino-platformio/)
* [Constantes pour déclarer le modèle dans le code Arduino](https://projetsdiy.fr/lilygowatch-ttgo-t-watch-projet-esp32-ide-arduino-platformio/#constantestwatch)
* [Constantes pour les périphériques et cartes d'extension](https://projetsdiy.fr/lilygowatch-ttgo-t-watch-projet-esp32-ide-arduino-platformio/#constantesaccessoirescartesextension)
* [Exemple de fichier platformio.ini](https://projetsdiy.fr/lilygowatch-ttgo-t-watch-projet-esp32-ide-arduino-platformio/#platformioinitwatch)
* [Acheter une T-Watch](https://projetsdiy.fr/boitier-montre-connectee-esp32-lilygo-ttgo-t-watch/)
* [Acheter une carte d'extension](https://projetsdiy.fr/boitier-montre-connectee-esp32-lilygo-ttgo-t-watch/#carteextensiontwatch) pour T-Watch, T-Watch-N, T-Block

## Développer une interface graphique avec TFT_eSPI ou LVGL
* [TFT_eSPI. Débuter avec la librairie. Afficher texte, formes, détection tactile](https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/)
* [Comment afficher des images XBM (TFT_eSPI) et C++ (LVGL)](https://projetsdiy.fr/ttgo-t-watch-afficher-images-xbm-tft_espi-lvgl-esp32-arduino/)
* [Mixer les librairies LVGL et TFT_eSPI dans un même projet ESP32](https://projetsdiy.fr/ttgo-t-watch-mixer-librairies-lvgl-tft_espi-projet-esp32/)

## Projets et jeux pour T-Watch
* [Dessiner des fractales de Mandelbrot ou Julia](https://projetsdiy.fr/ttgo-t-watch-projet-fractales-mandelbrot-julia-esp32/)

## Liens utiles
* [Site officiel de LilyGo](http://www.lilygo.cn/)
* [Dépot officiel de la librairie LiLyGoWatch](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)
* [LVGL](https://github.com/lvgl)
* [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
* [Documentation technique de l'AXP202, contrôleur d'alimentation](http://dl.linux-sunxi.org/AXP/AXP202%20Datasheet_v1.0_en.pdf)
* [Documentation technique du NXP PCF8563, horloge RTC](https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf)
* [Bosch Sensortec BMA423, accélèromètre 3 axes](https://www.bosch-sensortec.com/products/motion-sensors/accelerometers/bma423.html)
* [Librairie BMA423 développée par Bosch](https://github.com/BoschSensortec/BMA423-Sensor-API). [Adapation pour les T-Watch](https://github.com/lewisxhe/BMA423_Library)
* [GPS Seeed Studio Air 530](https://media.digikey.com/pdf/Data%20Sheets/Seeed%20Technology/109020022_Web.pdf) 
* [SIM800L, modem GPRS](https://simcom.ee/documents/SIM800/SIM800_Hardware%20Design_V1.09.pdf) 
* [SIM868, modem GPRS](https://simcom.ee/modules/gsm-gprs-gnss/sim868/) 
* [AcSIP S76GXB, SiP Lora + GPS](https://techship.com/products/acsip-s76gxb-developer-kit/) 

## Apprendre PlatformIO
Il est plus facile et rapide de développer le code Arduinoà l'aide de PlatformIO. Si vous débutez en programmation, vous pouvez commercer par lire ces articles
* [Installer PIO sur VSCode sur Windows, macOS ou Linux](https://projetsdiy.fr/debuter-platformio-ide-visual-studio-code-vscode-windows-linux/)
* [Platformio.ini, astuces et paramètres utiles. data_dir, monitor_speed, upload_port...](https://projetsdiy.fr/platformio-ini-astuces-parametres-connaitre/)
* [Problèmes et solutions courants](https://projetsdiy.fr/developper-platformio-problemes-solutions/)
* [Migrer du code Arduino vers un projet PlatformIO](https://projetsdiy.fr/comment-migrer-projet-esp8266-ide-arduino-platformio/)

_____

# Firmwares
A list of ready to use firmware you can install on your T-Watch 2020 or T-Watch Touch
* [T-Watch 2020 firmaware from Dirk Broßwick aka sharandac](https://github.com/sharandac/My-TTGO-Watch). WiFi, Bluetooth, IR Remote, OsmAnd, Clock, Stop Watch, LVGL, Accelerometer wakeup, scroll, BMA 423 step counter...
* [agoodWatch](https://github.com/AlexGoodyear/agoodWatch)
* [William F. Dudley aka wfdudley project](https://github.com/wfdudley/T-watch-2020) 

