
#include "QrCode.h"

int main()
{
    QrCode qr;
    qr.createQR("WASSUP", QrCode::CodingStatus::letters,QrCode::CorrectionLevel::H);
}
