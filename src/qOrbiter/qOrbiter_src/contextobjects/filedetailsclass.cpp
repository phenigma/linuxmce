
#include "filedetailsclass.h"
#include <qOrbiter/qOrbiter.h>

FileDetailsClass::FileDetailsClass(QObject* qorbiter_ptr, QObject *parent) :
    QObject(parent)
{
    qOrbiter*ptr = qobject_cast<qOrbiter*>(qorbiter_ptr);
    QObject::connect(ptr, &qOrbiter::fd_imageUrlChanged,this, &FileDetailsClass::setScreenshot);
    QObject::connect(ptr, &qOrbiter::fd_titleChanged,this, &FileDetailsClass::setTitle, Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::fd_storageDeviceChanged,this, &FileDetailsClass::setStorageDevice, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_mediaTitleChanged,this, &FileDetailsClass::setMediaTitle,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_directorChanged,this, &FileDetailsClass::setDirector,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_programChanged,this, &FileDetailsClass::setProgram,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_synopChanged,this, &FileDetailsClass::setSynop,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_albumChanged,this, &FileDetailsClass::setAlbum,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_performersChanged,this, &FileDetailsClass::setPerformers,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_genreChanged,this, &FileDetailsClass::setGenre,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_channelChanged,this, &FileDetailsClass::setChannel,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_pathChanged,this, &FileDetailsClass::setPath,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_fileChanged,this, &FileDetailsClass::setFile,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_fileNameChanged,this, &FileDetailsClass::setFilename,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_episodeChanged,this, &FileDetailsClass::setEpisode,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_trackChanged,this, &FileDetailsClass::setTrack,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_ratingChanged,this, &FileDetailsClass::setRating);
    QObject::connect(ptr,&qOrbiter::fd_studioChanged,this, &FileDetailsClass::setStudio);
    clear();
}



void FileDetailsClass::setFileMediaType()
{
}


void FileDetailsClass::clear(){
     setScreenshot("");


     emit screenShotChanged();
    directors.clear();
    composers.clear();
    composerlist.clear();
    performers.clear();
    performerlist.clear();
    qs_mainTitle.clear();
    qs_mainTitle2.clear();
    qs_subTitle.clear();
    mediatitle.clear();
    program.clear();
    channel.clear();
    channelID.clear();
    episode.clear();
    album.clear();
    track.clear();
    genre.clear();
    releasedate.clear();
    studio.clear();
    rating.clear();
    synop.clear();



    emit synopChanged();
    emit objectChanged();
    emit synopChanged();
    emit titleImageChanged();
    emit mediaTitleChanged();

    emit screenTypeChanged();
    emit titleChanged();
    emit titleChanged2();
    emit genreChanged();
    emit albumChanged();
    emit trackChanged();
    emit performersChanged();
    emit composersChanged();
    emit directorChanged();
    emit programChanged();
    emit channelChanged();
    emit episodeChanged();
    emit studioChanged();
    emit ratingChanged();
    emit screenShotChanged();

}
