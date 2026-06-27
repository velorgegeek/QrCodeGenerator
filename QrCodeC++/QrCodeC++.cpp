
#include "QrCode.h"

int main()
{
    QrCode qr;
    qr.createQR("QUIQIUQEUIQEIUQEIUQRIURQIUQRUIUIQRIUQRIUQRUIQRIUQRUIQRIUQRIUQRIUQRIUQIRUUDADDAADADDAADADADDA", QrCode::CodingStatus::letters,QrCode::CorrectionLevel::H);
}
