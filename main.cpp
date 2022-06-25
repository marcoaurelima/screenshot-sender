#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>

#include "functions.h"


int main(){

    screenshotToPixelArray(&scrshtInfo);

    image.create(scrshtInfo.screenSize.x, scrshtInfo.screenSize.y,sf::Color::Blue);
    sf::Texture texture;
    sf::Sprite sprite;

    networkInfo netInfo;
    netInfo.ip   = sf::IpAddress::getLocalAddress().toString();
    netInfo.port = 45000;

    std::stringstream line;
    for(int i=0;i<50;i++){line << static_cast<char>(205);}

    char c;
    printf(" %c%s%c", 201, line.str().c_str(), 187);
    printf("\n %c   Screenshot Sender          byMarco 17-01-2020  %c\n", 186, 186);
    printf(" %c%s%c", 200, line.str().c_str(),188);
    printf("\n     Seu IP:   %s\n", netInfo.ip.c_str());
    printf("\n     Enviar <e> Receber <r> ");

    scanf("%c", &c);


    if(c=='e'){
        printf("     IP: ");
        std::cin >> netInfo.ip;

        sendScreenInfo(netInfo);
        while(true){
            //Sleep(50);
            sendScreenshot(netInfo);
        }
        return 0;
    }

    printf("\n     Conectando... ");

    recvScreenInfo(netInfo);

    sf::Thread threadRecv(&recvScreenshot, netInfo);
    threadRecv.launch();

    sf::RenderWindow window(sf::VideoMode(600, 500), " ");
    window.setFramerateLimit(20);

    bool fullscreen = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::F2) && !fullscreen){
                window.create(sf::VideoMode(600, 500), " ",sf::Style::Fullscreen);
                fullscreen = !fullscreen;
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::F2) && fullscreen){
                window.create(sf::VideoMode(600, 500), " ",sf::Style::Default);
                fullscreen = !fullscreen;
            }
        }

        window.clear();


        texture.loadFromImage(::image);
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();
    }

    return 0;
}












