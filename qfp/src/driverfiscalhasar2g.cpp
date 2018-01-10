/*
*
* Copyright (C)2018, Samuel Isuani <sisuani@gmail.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* Neither the name of the project's author nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "driverfiscalhasar2g.h"
#include "packagefiscal.h"

#include <QCoreApplication>
#include <QDateTime>


DriverFiscalHasar2G::DriverFiscalHasar2G(QObject *parent, NetworkPort *m_networkPort, int m_TIME_WAIT)
    : QThread(parent), DriverFiscal(parent, m_networkPort, m_TIME_WAIT)
{
    m_nak_count = 0;
    m_error = false;
    errorHandler_count = 0;
    m_continue = true;
}

void DriverFiscalHasar2G::setModel(const FiscalPrinter::Model model)
{
    qDebug() << "MODEL: " << model << "ORIG: " << FiscalPrinter::Hasar615F;
    m_model = model;
}

void DriverFiscalHasar2G::finish()
{
    m_continue = false;
    queue.clear();
    quit();

    while(isRunning()) {
        msleep(100);
        quit();
    }
}

void DriverFiscalHasar2G::run()
{
    while(!queue.empty() && m_continue) {
        QVariantMap pkg = queue.first();
        QVariantMap result;

        /*

        QByteArray ret = readData(pkg->cmd(), 0);
        if(ret == "-1") {
            queue.clear();
            m_serialPort->readAll();

            if(errorHandler_count > 4)
                return;

            errorHandler_count++;

            errorHandler();

        } else if(!ret.isEmpty()) {
            if(ret.at(0) == PackageFiscal::NAK && m_nak_count <= 3) { // ! NAK
                m_nak_count++;
                SleeperThread::msleep(100);
                continue;
            }

            m_nak_count = 0;
            errorHandler_count = 0;


            if(pkg->cmd() == CMD_CLOSEFISCALRECEIPT || pkg->cmd() == CMD_CLOSEDNFH) {
                emit fiscalReceiptNumber(pkg->id(), getReceiptNumber(ret), pkg->ftype());
            }

            queue.pop_front();
            delete pkg;

        } else {

            queue.clear();
            //m_nak_count++;
        }
        */
    }
}

void DriverFiscalHasar2G::errorHandler()
{

}

int DriverFiscalHasar2G::getReceiptNumber(const QByteArray &data)
{
    /*
    QByteArray tmp = data;
    tmp.remove(0, 14);
    for(int i = 0; i < tmp.size(); i++) {
        if(m_model == FiscalPrinter::Hasar330F) {
            if(tmp.at(i) == PackageFiscal::FS) {
                tmp.remove(i, tmp.size());
                break;
            }
        } else {
            if(tmp.at(i) == PackageFiscal::ETX) {
                tmp.remove(i, tmp.size());
                break;
            }
        }
    }

#if LOGGER
    qDebug() << QString("F. Num: %1").arg(tmp.trimmed().toInt());
#endif
    return tmp.trimmed().toInt();
    */
}

bool DriverFiscalHasar2G::getStatus(const QByteArray &data)
{
    /*
    QByteArray tmp = data;
    tmp.remove(0, 11);
    tmp.remove(5, tmp.size());

    return tmp == QByteArray("0000");
    */
}

QByteArray DriverFiscalHasar2G::readData(const int pkg_cmd, const QByteArray &secuence)
{
    /*
    bool ok = false;
    int count_tw = 0;
    QByteArray bytes;

    do {
        if(m_serialPort->bytesAvailable() <= 0 && m_continue) {
            qDebug() << "NB tw";
            SleeperThread::msleep(100);
        }

        if(!m_continue)
            return "";

        QByteArray bufferBytes = m_serialPort->read(1);
        if(bufferBytes.at(0) == PackageFiscal::ACK) {
            //qDebug() << QString("ACK PRINTER: %1 ").arg(bufferBytes.toHex());
            SleeperThread::msleep(100);
        } else if(bufferBytes.at(0) == PackageFiscal::DC1 || bufferBytes.at(0) == PackageFiscal::DC2
                || bufferBytes.at(0) == PackageFiscal::DC3 || bufferBytes.at(0) == PackageFiscal::DC4
                || bufferBytes.at(0) == PackageFiscal::ACK) {
            qDebug() << "DC tw: " << bufferBytes.toHex();
            SleeperThread::msleep(100);
            count_tw -= 30;
            continue;
        } else if(bufferBytes.at(0) == PackageFiscal::NAK) {
            qDebug() << QString("NAK");
            return bufferBytes;
        } else if(bufferBytes.at(0) == PackageFiscal::STX) {
            bytes += PackageFiscal::STX;

            bufferBytes = m_serialPort->read(1);
            while(bufferBytes.at(0) != PackageFiscal::ETX) {
                count_tw++;
                if(bufferBytes.isEmpty()) {
                    SleeperThread::msleep(100);
                }

                bytes += bufferBytes;
                bufferBytes = m_serialPort->read(1);

                if(count_tw >= MAX_TW)
                    break;
            }

            bytes += PackageFiscal::ETX;

            QByteArray checkSumArray;
            int checksumCount = 0;
            while(checksumCount != 4) {
                checkSumArray = m_serialPort->read(1);
                count_tw++;
                if(checkSumArray.isEmpty()) {
                    SleeperThread::msleep(100);
                } else {
                    checksumCount++;
                    bytes += checkSumArray;
                }

                if(count_tw >= MAX_TW)
                    break;
            }

            ok = true;
            break;
        } else {
            bytes += bufferBytes;
        }
        count_tw++;

    } while(ok != true && count_tw <= MAX_TW && m_continue);

    qDebug() << QString("COUNTER: %1 %2").arg(count_tw).arg(MAX_TW);

    ok = verifyResponse(bytes, pkg_cmd);

    if(!ok) {
        qDebug() << QString("ERRRRRRRRRRRRRRRRRRRRRRROOOOOOOOOOOOOOOOOORRRRRRRRRRRR READ: %1").arg(bytes.toHex().data());
        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            m_serialPort->readAll();
            emit fiscalStatus(FiscalPrinter::Error);
        }
        return "-1";
    } else {
        qDebug() << QString("--> OK PKGV3: %1").arg(bytes.toHex().data());
        emit fiscalStatus(FiscalPrinter::Ok);
    }

    return bytes;
    */
}

bool DriverFiscalHasar2G::verifyResponse(const QByteArray &bytes, const int pkg_cmd)
{
    /*
    if(bytes.at(0) != PackageFiscal::STX) {
        qDebug() << QString("NO STX %1").arg(bytes.toHex().data());
        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            emit fiscalStatus(FiscalPrinter::Error);
        }
        return false;
    }

    if(QChar(bytes.at(2)).unicode() != pkg_cmd) {
        qDebug() << QString("ERR - diff cmds: %1 %2").arg(QChar(bytes.at(2)).unicode()).arg(pkg_cmd);
        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            emit fiscalStatus(FiscalPrinter::Error);
        }

        return false;
    }

    m_error = false;

    if(bytes.at(3) != PackageFiscal::FS) {
        qDebug() << QString("Error: diff FS");

        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            emit fiscalStatus(FiscalPrinter::Error);
        }
        return false;
    }

    if(bytes.at(bytes.size() - 5) != PackageFiscal::ETX) {
        qDebug() << QString("Error: ETX");

        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            emit fiscalStatus(FiscalPrinter::Error);
        }
        return false;
    }

    return true;

    if(!checkSum(bytes)) {
        qDebug() << QString("Error: checksum");

        if(pkg_cmd == 42) {
            qDebug() << QString("SIGNAL ->> ENVIO STATUS ERROR");
            emit fiscalStatus(FiscalPrinter::Error);
        }
        return false;
    }

    return true;
    */
}

bool DriverFiscalHasar2G::checkSum(const QString &data)
{
    /*
    bool ok;
    int checkSum = data.right(4).toInt(&ok, 16);
    if(!ok) {
        return false;
    }

    int ck_cmd = 0;
    for(int i = 0; i < data.size()-4; i++) {
        ck_cmd += QChar(data.at(i)).unicode();
    }


    return (checkSum == ck_cmd);
    */
}

void DriverFiscalHasar2G::statusRequest()
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_STATUS);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::dailyClose(const char type)
{
    QVariantMap d;
    QVariantMap report;

    report["Reporte"] = type == 'Z' ? "ReporteZ" : "ReporteX";
    d["CerrarJornadaFiscal"] = report;

    queue.append(d);

    QVariantMap pkg = queue.first();
    QVariantMap result;
    m_networkPort->post(pkg, &result);
    queue.pop_front();

    //start();
}

void DriverFiscalHasar2G::dailyCloseByDate(const QDate &from, const QDate &to)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_DAILYCLOSEBYDATE);

    QByteArray d;
    d.append(from.toString("yyMMdd"));
    d.append(PackageFiscal::FS);
    d.append(to.toString("yyMMdd"));
    d.append(PackageFiscal::FS);
    d.append('T');
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::dailyCloseByNumber(const int from, const int to)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_DAILYCLOSEBYNUMBER);

    QByteArray d;
    d.append(QString::number(from));
    d.append(PackageFiscal::FS);
    d.append(QString::number(to));
    d.append(PackageFiscal::FS);
    d.append('T');
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::setCustomerData(const QString &name, const QString &cuit, const char tax_type,
        const QString &doc_type, const QString &address)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_SETCUSTOMERDATA);

    QByteArray d;
    if(m_model == FiscalPrinter::Hasar330F || m_model == FiscalPrinter::Hasar320F
            || m_model == FiscalPrinter::Hasar715F) {
        d.append(name.left(49));
        d.append(PackageFiscal::FS);
        d.append(cuit);
        d.append(PackageFiscal::FS);
        d.append(tax_type);
        d.append(PackageFiscal::FS);
        d.append(doc_type);
        d.append(PackageFiscal::FS);
        d.append(address.left(50));
    } else {
        d.append(name.left(50));
        d.append(PackageFiscal::FS);
        d.append(cuit);
        d.append(PackageFiscal::FS);
        d.append(tax_type);
        d.append(PackageFiscal::FS);
        d.append(doc_type);
        if(m_model == FiscalPrinter::Hasar715F) {
            d.append(PackageFiscal::FS);
            d.append(address.left(40));
        } else {
            /// setHeaderTrailer??
        }
    }
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::openFiscalReceipt(const char type)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_OPENFISCALRECEIPT);

    QByteArray d;
    d.append(type);
    d.append(PackageFiscal::FS);
    d.append('T');
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::printFiscalText(const QString &text)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_PRINTFISCALTEXT);

    QByteArray d;
    d.append(text.left(50));
    d.append(PackageFiscal::FS);
    d.append("0");
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::printLineItem(const QString &description, const qreal quantity,
        const qreal price, const QString &tax, const char qualifier, const qreal excise)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_PRINTLINEITEM);

    QByteArray d;
    if(m_model == FiscalPrinter::Hasar615F || m_model == FiscalPrinter::Hasar715F)
        d.append(description.left(18));
    else
        d.append(description.left(62));
    d.append(PackageFiscal::FS);
    d.append(QString::number(quantity, 'f', 2));
    d.append(PackageFiscal::FS);
    d.append(QString::number(price, 'f', 2));
    d.append(PackageFiscal::FS);
    d.append(tax);
    d.append(PackageFiscal::FS);
    d.append(qualifier);
    d.append(PackageFiscal::FS);
    d.append("0.00");
    d.append(PackageFiscal::FS);
    d.append("0");
    d.append(PackageFiscal::FS);
    d.append('T');
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::perceptions(const QString &desc, qreal tax_amount)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_PERCEPTIONS);

    QByteArray d;
    d.append("**.**");
    d.append(PackageFiscal::FS);
    d.append(desc);
    d.append(PackageFiscal::FS);
    d.append(QString::number(tax_amount, 'f', 2));
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::subtotal(const char print)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_SUBTOTAL);

    QByteArray d;
    d.append(print);
    d.append(PackageFiscal::FS);
    d.append("Subtotal");
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::totalTender(const QString &description, const qreal amount, const char type)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_TOTALTENDER);

    QByteArray d;
    d.append(description.left(49));
    d.append(PackageFiscal::FS);
    d.append(QString::number(amount, 'f', 2));
    d.append(PackageFiscal::FS);
    d.append(type);
    if(m_model == FiscalPrinter::Hasar615F || m_model == FiscalPrinter::Hasar715F) {
        d.append(PackageFiscal::FS);
        d.append("0");
    }
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::generalDiscount(const QString &description, const qreal amount, const qreal tax_percent, const char type)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_GENERALDISCOUNT);

    QByteArray d;
    d.append(description.left(49));
    d.append(PackageFiscal::FS);
    d.append(QString::number(amount, 'f', 2));
    d.append(PackageFiscal::FS);
    d.append(type);
    d.append(PackageFiscal::FS);
    d.append("0");
    d.append(PackageFiscal::FS);
    d.append('T');

    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::closeFiscalReceipt(const char intype, const char f_type, const int id)
{
    /*
    Q_UNUSED(intype);
    Q_UNUSED(f_type);

    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_CLOSEFISCALRECEIPT);
    if(f_type == 'S')
        p->setFtype(0);
    else if(f_type == 'r')
        p->setFtype(3);
    p->setId(id);
    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::openNonFiscalReceipt()
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_OPENNONFISCALRECEIPT);

    QByteArray d;
    d.append(" ");
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::printNonFiscalText(const QString &text)
{
    /*
    QString t = text;
    while(!t.isEmpty()) {
        PackageHasar *p = new PackageHasar;
        p->setCmd(CMD_PRINTNONTFISCALTEXT);
        p->setData(t.left(39));
        t.remove(0, 39);
        queue.append(p);
    }

    start();
    */
}

void DriverFiscalHasar2G::closeNonFiscalReceipt()
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_CLOSENONFISCALRECEIPT);

    QByteArray d;
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::openDrawer()
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_OPENDRAWER);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::setHeaderTrailer(const QString &header, const QString &trailer)
{
    // HEADER is set
    /*
    if(!header.isEmpty()) {
        PackageHasar *p = new PackageHasar;
        p->setCmd(CMD_SETHEADERTRAILER);

        QByteArray d;
        d.append("1");
        d.append(PackageFiscal::FS);
        d.append(header.left(120));
        p->setData(d);

        queue.append(p);
        start();
    }*/

    /*

    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_SETHEADERTRAILER);
    QByteArray d;
    d.append("12");
    d.append(PackageFiscal::FS);

    if(!trailer.isEmpty()) {
        d.append(trailer.left(120));
    } else {
        d.append(0x7f);
    }

    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::setEmbarkNumber(const int doc_num, const QString &description)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_EMBARKNUMBER);

    QByteArray d;
    d.append(QString::number(doc_num));
    d.append(PackageFiscal::FS);
    d.append(description);
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::openDNFH(const char type, const char fix_value, const QString &doc_num)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_OPENDNFH);

    QByteArray d;
    d.append(type);
    d.append(PackageFiscal::FS);
    d.append(fix_value);

    if(m_model == FiscalPrinter::Hasar330F || m_model == FiscalPrinter::Hasar320F) {
        d.append(PackageFiscal::FS);
        d.append(doc_num);
    }
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::printEmbarkItem(const QString &description, const qreal quantity)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_PRINTEMBARKITEM);

    QByteArray d;
    d.append(description);
    d.append(PackageFiscal::FS);
    d.append(QString::number(quantity, 'f', 4));
    d.append(PackageFiscal::FS);
    d.append("0");
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::closeDNFH(const int id, const char f_type, const int copies)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_CLOSEDNFH);

    if(m_model == FiscalPrinter::Hasar330F || m_model == FiscalPrinter::Hasar320F) {
        QByteArray d;
        d.append(QString::number(copies));
        p->setData(d);
    }
    p->setFtype(f_type == 'R' ? 1 : 2);
    p->setId(id);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::cancel()
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_CANCEL);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::ack()
{
}

void DriverFiscalHasar2G::setDateTime(const QDateTime &dateTime)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_SETDATETIME);
    QByteArray d;
    d.append(dateTime.date().toString("yyMMdd"));
    d.append(PackageFiscal::FS);
    d.append(dateTime.time().toString("HHmmss"));
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::receiptText(const QString &text)
{
    /*
    PackageHasar *p = new PackageHasar;
    p->setCmd(CMD_RECEIPTTEXT);
    QByteArray d;
    d.append(text.left(100));
    p->setData(d);

    queue.append(p);
    start();
    */
}

void DriverFiscalHasar2G::reprintDocument(const QString &doc_type, const int doc_number)
{
}

void DriverFiscalHasar2G::reprintContinue()
{
}

void DriverFiscalHasar2G::reprintFinalize()
{
}

void DriverFiscalHasar2G::setFixedData(const QString &shop, const QString &phone)
{
    /*
    QByteArray d;

    PackageHasar *pp = new PackageHasar;
    pp->setCmd(0x5d);

    d.append("11");
    d.append(PackageFiscal::FS);
    d.append("DEFENSA CONSUMIDOR " + phone);
    pp->setData(d);
    queue.append(pp);
    d.clear();

    PackageHasar *p1 = new PackageHasar;
    p1->setCmd(0x5d);

    d.append("12");
    d.append(PackageFiscal::FS);
    d.append("Entra con este ticket a:");
    p1->setData(d);
    queue.append(p1);
    d.clear();

    PackageHasar *p2 = new PackageHasar;
    p2->setCmd(0x5d);

    d.append("13");
    d.append(PackageFiscal::FS);
    d.append("www.cuentaleasubway.com y llevate una");
    p2->setData(d);
    queue.append(p2);
    d.clear();

    PackageHasar *p3 = new PackageHasar;
    p3->setCmd(0x5d);

    d.append("14");
    d.append(PackageFiscal::FS);
    d.append("COOKIE GRATIS, Rest. ID: " + shop);
    p3->setData(d);
    queue.append(p3);
    d.clear();


    start();
    */
}

void DriverFiscalHasar2G::getTransactionalMemoryInfo()
{
}

void DriverFiscalHasar2G::downloadReportByDate(const QString &type, const QDate &form, const QDate &to)
{
}

void DriverFiscalHasar2G::downloadReportByNumber(const QString &type, const int from, const int to)
{
}

void DriverFiscalHasar2G::downloadContinue()
{
}

void DriverFiscalHasar2G::downloadFinalize()
{
}

void DriverFiscalHasar2G::downloadDelete(const int to)
{
}
