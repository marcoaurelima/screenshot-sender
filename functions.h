#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include<SFML/Network.hpp>
typedef struct {

    char *pixelArray;
    int pixelArraySize = 0;
    sf::Vector2u screenSize = sf::Vector2u(1,1);

} screenshotInfo;



typedef struct {

    std::string ip;
    int port;

} networkInfo;



void screenshotToPixelArray(screenshotInfo *scrshotInfo){

    HDC hdcScr, hdcMem;
    HBITMAP hbmScr;
    //BITMAP bmp;
    int iXRes, iYRes;

    hdcScr = CreateDC("DISPLAY", NULL, NULL, NULL);
    hdcMem = CreateCompatibleDC(hdcScr);

    iXRes = GetDeviceCaps(hdcScr, HORZRES);
    iYRes = GetDeviceCaps(hdcScr, VERTRES);

    hbmScr = CreateCompatibleBitmap(hdcScr, iXRes, iYRes);

    SelectObject(hdcMem, hbmScr);

    StretchBlt(hdcMem,
        0, 0, iXRes, iYRes,
        hdcScr,
        0, 0, iXRes, iYRes,
        SRCCOPY);

    int size = GetBitmapBits(hbmScr,0,NULL); /// Retorna o tamanho da imagem
    char *pBuffer = new char[size];

    GetBitmapBits(hbmScr,size,pBuffer);

    scrshotInfo->pixelArray = pBuffer;
    scrshotInfo->pixelArraySize = size;
    scrshotInfo->screenSize.x = iXRes;
    scrshotInfo->screenSize.y = iYRes;


    DeleteDC(hdcScr);
    DeleteDC(hdcMem);
    DeleteObject(hbmScr);

}


sf::Image pixelArrayToSFImage(char *pBuffer, sf::Vector2u screenSize){
    sf::Image image;
    image.create(screenSize.x,screenSize.y,sf::Color::Black);

    unsigned int strIndex = 0;
    for(size_t i=0;i<screenSize.y;i++){
        for(size_t j=0;j<screenSize.x;j++){
            sf::Color pixel;
            unsigned char r = pBuffer[strIndex  ];
            unsigned char g = pBuffer[strIndex+1];
            unsigned char b = pBuffer[strIndex+2];
            pixel = sf::Color(b,g,r,255);

            image.setPixel(j,i,pixel);
            strIndex = strIndex + 4;
        }
    }

    return image;
}

char* SFImageToPixelAray(sf::Image image, const int sizePixelArray){
    const int larg = image.getSize().x;
    const int alt = image.getSize().y;


    char *arrayPixels = (char*) calloc(sizePixelArray, sizeof(char));

    std::stringstream allPixels;
    int index = 0;
    for(int i=0;i<alt;i++){
        for(int j=0;j<larg;j++){
            sf::Color pixel;
            pixel = image.getPixel(j,i);

            char r = pixel.r;
            char g = pixel.g;
            char b = pixel.b;
            char a = 255;
            arrayPixels[index] = b; index++;
            arrayPixels[index] = g; index++;
            arrayPixels[index] = r; index++;
            arrayPixels[index] = a; index++;

        }
    }

    return arrayPixels;
}


///GLOBAL
static screenshotInfo scrshtInfo;
static sf::Image image;

void sendScreenInfo(networkInfo netInfo){

    screenshotToPixelArray(&scrshtInfo);

    sf::TcpSocket socket;
    sf::Socket::Status status;

    status = socket.connect(netInfo.ip, netInfo.port);
    if(status == sf::Socket::Status::Done){ printf("\n     [x] conectado  \n"); } else { printf("\n  erro coneccao : \n");}

    std::stringstream ss;
    ss << scrshtInfo.screenSize.x << " " << scrshtInfo.screenSize.y << " " << scrshtInfo.pixelArraySize << "  \0";
    //printf("Mensagem: %s", ss.str().c_str());


    //size_t sent;
    socket.send(ss.str().c_str(), 30);
    if(status == sf::Socket::Status::Done){ /*printf("\n  enviadoInfo : %d\n", sent);*/ } else { printf("\n  erro eviar info : \n");}
    socket.disconnect();

}



void recvScreenInfo(networkInfo netInfo){


    sf::TcpSocket socket;
    sf::TcpListener listener;

    listener.listen(netInfo.port);
    listener.accept(socket);


    size_t received = 0;
    char buffer[30];
    sf::Socket::Status status = socket.receive(buffer,30,received);
    socket.disconnect();

    if(status == sf::Socket::Status::Done){/* printf("\n\n   Recebido : %d %s\n...", received, buffer);*/ } else { printf("\n  erro receiver : ");}

    sscanf(buffer,"%u %u %u", &scrshtInfo.screenSize.x, &scrshtInfo.screenSize.y, &scrshtInfo.pixelArraySize);
    socket.disconnect();

}



void sendScreenshot(networkInfo netInfo){

    static sf::TcpSocket socket;

    socket.connect(netInfo.ip, netInfo.port);

    //if(status == sf::Socket::Status::Done){ printf("\n  conectado : "); } else { printf("\n  erro coneccao : ");}

    screenshotToPixelArray(&scrshtInfo);
    socket.send(scrshtInfo.pixelArray,scrshtInfo.pixelArraySize);

    //if(status == sf::Socket::Status::Done){ printf("\n  enviado : "); } else { printf("\n  erro send : ");}

    free(scrshtInfo.pixelArray);
    scrshtInfo.pixelArray = NULL;
}



void recvScreenshot(networkInfo netInfo){

    while(true){
        static sf::TcpSocket socket;
        static sf::TcpListener listener;

        listener.listen(netInfo.port);
        listener.accept(socket);


        scrshtInfo.pixelArray = (char*) calloc(scrshtInfo.pixelArraySize, sizeof(char));

        size_t received = 0;
        sf::Socket::Status status = socket.receive(scrshtInfo.pixelArray,scrshtInfo.pixelArraySize,received);
        socket.disconnect();

        //if(status == sf::Socket::Status::Done){ printf("\n  recebido : "); } else { printf("\n  erro receiver : ");}

        if(status == sf::Socket::Status::Done){
            ::image = pixelArrayToSFImage(scrshtInfo.pixelArray,scrshtInfo.screenSize);
        }

        free(scrshtInfo.pixelArray);
        scrshtInfo.pixelArray = NULL;
    }

}



























#endif // FUNCTIONS_H_INCLUDED
