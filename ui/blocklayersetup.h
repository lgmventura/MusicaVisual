#ifndef BLOCKLAYERSETUP_H
#define BLOCKLAYERSETUP_H

#include <QDockWidget>

#include "visual/tracks.h"
#include "musicdata.h"

namespace Ui {
class BlockLayerSetup;
}

class BlockLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit BlockLayerSetup(MusicData *mdt, TracksP *tProp, QWidget *parent = nullptr);
    ~BlockLayerSetup();

private:
    Ui::BlockLayerSetup *ui;

    TracksP *TProp;
    MusicData *Mdt;
};

#endif // BLOCKLAYERSETUP_H
