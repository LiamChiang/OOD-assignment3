#ifndef STAGETHREEFACTORY_H
#define STAGETHREEFACTORY_H
#pragma once

#include "abstractstagefactory.h"
#include <QJsonObject>


class StageThreeFactory : public AbstractStageFactory
{
public:
    StageThreeFactory() {}
    /**
     * @brief makeBall - construct a ball based on json
     * @param ballData - our json data for this table
     * @return
     */
    virtual Ball* makeBall(const QJsonObject& ballData) override;

    /**
     * @brief makeTable - construct a table based on json
     * @param tableData - our json data for this table
     * @return
     */
    virtual Table* makeTable(const QJsonObject& tableData) override;

    /**
     * @brief makePocket - construct a pocket based on json
     * @param pocketData - the json
     * @return newly created pocket
     */
    virtual Pocket* makePocket(const QJsonObject &pocketData) override;
};

#endif // STAGETHREEFACTORY_H
