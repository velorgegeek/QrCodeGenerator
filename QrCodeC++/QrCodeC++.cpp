#include <iostream>
#include "QrCode.h"

int main()
{
    QrCode qr;
    qr.createQR("ALEK OS", QrCode::CodingStatus::letters,QrCode::CorrectionLevel::Q);
}
