#ifndef PROJECTINFO_H
#define PROJECTINFO_H

#include <vector>
#include <QString>

using std::vector;

struct ProjectInfo
{
    int                 dataloaderCnt = {0};
    int                 unetCnt = {0};
    int                 lossCnt = {0};
    int                 optCnt = {0};
    int                 trainCnt = {0};
    int                 testCnt = {0};

    int                 dataloader2unet = {0};

    vector<QString>     lossInputErrs;
    vector<QString>     lossOutputErrs;
    vector<QString>     optOutputErrs;
};

#endif // PROJECTINFO_H
