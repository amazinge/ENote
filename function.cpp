#include "function.h"

QString function::removeHtml(QString html)
{
    QRegExp regexp("<(?!img|br|p|/p|style|/style|span|/span).*>");
    regexp.setCaseSensitivity(Qt::CaseInsensitive);
    regexp.setMinimal(true);
    QString ret=html;
    ret=ret.replace(regexp,"");
    return ret;
}
