#include "QPlayerWidget.h"

QPlayerWidget::QPlayerWidget(QWidget *parent) : QWidget(parent),
    p_ffmpeg_player(new FFmpegPlayer())
{
    // Set size and background color
    qDebug() << "FFmpeg version: " << av_version_info();
    setFixedSize(640, 480);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setAutoFillBackground(true);
    setPalette(palette);

    p_ffmpeg_player->set_preview_callback([&](uint8_t *data,int width,int height){
        qDebug() << "preview_callback";
        if(!frame_avaliable.load(std::memory_order_acquire))
        {
            QImage frame_image(data, width, height, QImage::Format_RGBA8888);
            m_image = frame_image.copy();
            this->update();
        }
        frame_avaliable.store(true, std::memory_order_release);
    });
}
void QPlayerWidget::paintEvent(QPaintEvent *event)
{
    qDebug() << __FUNCTION__;
    if(frame_avaliable.load(std::memory_order_acquire))
    {
        QPainter painter(this);
        painter.drawImage(rect(), m_image);
    }
    frame_avaliable.store(false, std::memory_order_release);
}
void QPlayerWidget::start_preview(const std::string &media_url)
{
    p_ffmpeg_player->start_preview(media_url);
}
void QPlayerWidget::stop_preview()
{
    p_ffmpeg_player->stop_preview();
}
