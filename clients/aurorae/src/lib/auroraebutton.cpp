/*
    Library for Aurorae window decoration themes.
    Copyright (C) 2009, 2010 Martin Gräßlin <kde@martin-graesslin.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "auroraebutton.h"
#include "auroraescene.h"
#include "auroraetheme.h"
#include "themeconfig.h"
// Qt
#include <QtCore/QEasingCurve>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>
#include <QtGui/QGraphicsSceneEvent>
#include <QtGui/QPainter>
// KDE
#include <KDE/KIcon>
#include <KDE/KIconLoader>
#include <KDE/KIconEffect>
#include <KDE/Plasma/FrameSvg>
#include <KDE/Plasma/PaintUtils>

namespace Aurorae {

AuroraeButton::AuroraeButton(AuroraeTheme* theme, AuroraeButtonType type)
    : QGraphicsWidget()
    , m_theme(theme)
    , m_type(type)
    , m_pressed(false)
    , m_hovered(false)
    , m_animationProgress(0.0)
    , m_animation(new QPropertyAnimation(this, "animation", this))
    , m_checkable(false)
    , m_checked(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

AuroraeButton::~AuroraeButton()
{
}

void AuroraeButton::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (m_theme->hasButton(m_type)) {
        ButtonStates state;
        if (static_cast<AuroraeScene*>(scene())->isActive()) {
            state |= Active;
        }
        if (m_hovered) {
            state |= Hover;
        }
        if (m_pressed) {
            state |= Pressed;
        }
        if (isCheckable() && isChecked()) {
            state |= Pressed;
        }
        paintButton(painter, m_theme->button(currentType()), state);
    }
}

AuroraeButtonType AuroraeButton::currentType() const
{
    return m_type;
}

QSizeF AuroraeButton::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    Q_UNUSED(which)
    Q_UNUSED(constraint)
    switch (m_type) {
    case MinimizeButton:
        return QSizeF(m_theme->themeConfig().buttonWidthMinimize(),
                      m_theme->themeConfig().buttonHeight());
    case MaximizeButton:
    case RestoreButton:
        return QSizeF(m_theme->themeConfig().buttonWidthMaximizeRestore(),
                      m_theme->themeConfig().buttonHeight());
    case CloseButton:
        return QSizeF(m_theme->themeConfig().buttonWidthClose(),
                      m_theme->themeConfig().buttonHeight());
    case AllDesktopsButton:
        return QSizeF(m_theme->themeConfig().buttonWidthAllDesktops(),
                      m_theme->themeConfig().buttonHeight());
    case KeepAboveButton:
        return QSizeF(m_theme->themeConfig().buttonWidthKeepAbove(),
                      m_theme->themeConfig().buttonHeight());
    case KeepBelowButton:
        return QSizeF(m_theme->themeConfig().buttonWidthKeepBelow(),
                      m_theme->themeConfig().buttonHeight());
    case ShadeButton:
        return QSizeF(m_theme->themeConfig().buttonWidthShade(),
                      m_theme->themeConfig().buttonHeight());
    case HelpButton:
        return QSizeF(m_theme->themeConfig().buttonWidthHelp(),
                      m_theme->themeConfig().buttonHeight());
    default:
        return QSizeF(m_theme->themeConfig().buttonWidth(), m_theme->themeConfig().buttonHeight());
    }
}
void AuroraeButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    m_pressed = true;
    update();
}

void AuroraeButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    if (m_pressed) {
        emit clicked();
    }
    m_pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void AuroraeButton::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    m_hovered = true;
    if (isAnimating()) {
        m_animation->stop();
    }
    m_animationProgress = 0.0;
    int time = m_theme->themeConfig().animationTime();
    if (time != 0) {
        m_animation->setDuration(time);
        m_animation->setEasingCurve(QEasingCurve::InQuad);
        m_animation->setStartValue(0.0);
        m_animation->setEndValue(1.0);
        m_animation->start();
    }
    update();
}

void AuroraeButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    m_hovered = false;
    if (isAnimating()) {
        m_animation->stop();
    }
    m_animationProgress = 0.0;
    int time = m_theme->themeConfig().animationTime();
    if (time != 0) {
        m_animation->setDuration(time);
        m_animation->setEasingCurve(QEasingCurve::OutQuad);
        m_animation->setStartValue(0.0);
        m_animation->setEndValue(1.0);
        m_animation->start();
    }
    update();
}

void AuroraeButton::paintButton(QPainter *painter, Plasma::FrameSvg *frame, ButtonStates states)
{
    QString prefix = "active";
    QString animationPrefix = "active";
    bool hasInactive = false;
    // check for inactive prefix
    if (!states.testFlag(Active) && frame->hasElementPrefix("inactive")) {
        // we have inactive, so we use it
        hasInactive = true;
        prefix = "inactive";
        animationPrefix = "inactive";
    }

    if (states.testFlag(Hover)) {
        if (states.testFlag(Active)) {
            if (frame->hasElementPrefix("hover")) {
                prefix = "hover";
            }
        } else {
            if (hasInactive) {
                if (frame->hasElementPrefix("hover-inactive")) {
                    prefix = "hover-inactive";
                }
            } else {
                if (frame->hasElementPrefix("hover")) {
                    prefix = "hover";
                }
            }
        }
    }
    if (states.testFlag(Pressed)) {
        if (states.testFlag(Active)) {
            if (frame->hasElementPrefix("pressed")) {
                prefix = "pressed";
            }
        } else {
            if (hasInactive) {
                if (frame->hasElementPrefix("pressed-inactive")) {
                    prefix = "pressed-inactive";
                }
            } else {
                if (frame->hasElementPrefix("pressed")) {
                    prefix = "pressed";
                }
            }
        }
    }
    if (states.testFlag(Deactivated)) {
        if (states.testFlag(Active)) {
            if (frame->hasElementPrefix("deactivated")) {
                prefix = "deactivated";
            }
        } else {
            if (hasInactive) {
                if (frame->hasElementPrefix("deactivated-inactive")) {
                    prefix = "deactivated-inactive";
                }
            } else {
                if (frame->hasElementPrefix("deactivated")) {
                    prefix = "deactivated";
                }
            }
        }
    }
    frame->setElementPrefix(prefix);
    frame->resizeFrame(size());
    if (isAnimating()) {
        // there is an animation so we have to use it
        // the animation is definately a hover animation as currently nothing else is supported
        if (!states.testFlag(Hover)) {
            // only have to set for not hover state as animationPrefix is set to (in)active by default
            if (states.testFlag(Active)) {
                if (frame->hasElementPrefix("hover")) {
                    animationPrefix = "hover";
                }
            } else {
                if (hasInactive) {
                    if (frame->hasElementPrefix("hover-inactive")) {
                        animationPrefix = "hover-inactive";
                    }
                } else {
                    if (frame->hasElementPrefix("hover")) {
                        animationPrefix = "hover";
                    }
                }
            }
        }
        QPixmap target = frame->framePixmap();
        frame->setElementPrefix(animationPrefix);
        frame->resizeFrame(size());
        QPixmap result = Plasma::PaintUtils::transition(frame->framePixmap(),
                                                        target, m_animationProgress);
        painter->drawPixmap(QRect(QPoint(0, 0), size().toSize()), result);
    } else {
        bool animation = false;
        AuroraeScene *s = static_cast<AuroraeScene*>(scene());
        if (s->isAnimating()) {
            animationPrefix = prefix;
            if (prefix.endsWith("-inactive")) {
                animationPrefix.remove("-inactive");
            } else {
                animationPrefix = animationPrefix + "-inactive";
            }
            if (frame->hasElementPrefix(animationPrefix)) {
                animation = true;
                QPixmap target = frame->framePixmap();
                frame->setElementPrefix(animationPrefix);
                frame->resizeFrame(size());
                QPixmap result = Plasma::PaintUtils::transition(frame->framePixmap(),
                                                                target, s->animationProgress());
                painter->drawPixmap(0, 0, result);
            }
        }
        if (!animation) {
            frame->paintFrame(painter);
        }
    }
}

bool AuroraeButton::isAnimating() const
{
    return (m_animation->state() == QAbstractAnimation::Running);
}

qreal AuroraeButton::animationProgress() const
{
    return m_animationProgress;
}

void AuroraeButton::setAnimationProgress(qreal progress)
{
    m_animationProgress = progress;
    update();
}

/***********************************************
* AuroraeMaximizeButton
***********************************************/
AuroraeMaximizeButton::AuroraeMaximizeButton(AuroraeTheme* theme)
    : AuroraeButton(theme, MaximizeButton)
    , m_maximizeMode(KDecorationDefines::MaximizeRestore)
{
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton|Qt::MidButton);
}

void AuroraeMaximizeButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    AuroraeButton::mousePressEvent(event);
    m_pressedButton = event->button();
}

void AuroraeMaximizeButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (isPressed() && m_pressedButton == event->button()) {
        emit clicked(m_pressedButton);
    }
    setPressed(false);
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

AuroraeButtonType AuroraeMaximizeButton::currentType() const
{
    if (m_maximizeMode == KDecorationDefines::MaximizeFull &&
        theme()->hasButton(RestoreButton)) {
        return RestoreButton;
    } else {
        return MaximizeButton;
    }
}

void AuroraeMaximizeButton::setMaximizeMode(KDecorationDefines::MaximizeMode mode)
{
    m_maximizeMode = mode;
    update();
}

/************************************************
* AuroraeMenuButton
************************************************/
AuroraeMenuButton::AuroraeMenuButton(AuroraeTheme* theme)
    : AuroraeButton(theme, MenuButton)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(QApplication::doubleClickInterval());
    connect(m_timer, SIGNAL(timeout()), SIGNAL(clicked()));
}

void AuroraeMenuButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (m_icon.isNull()) {
        return;
    }
    QPixmap iconPix = m_icon;
    KIconEffect *effect = KIconLoader::global()->iconEffect();
    AuroraeScene *s = static_cast<AuroraeScene*>(scene());
    if (s->isActive()) {
        if (isHovered()) {
            iconPix = effect->apply(iconPix, KIconLoader::Desktop, KIconLoader::ActiveState);
        }
    } else {
        iconPix = effect->apply(iconPix, KIconLoader::Desktop, KIconLoader::DisabledState);
    }
    if (s->isAnimating()) {
        // animation
        QPixmap oldPix = m_icon;
        if (!s->isActive()) {
            if (isHovered()) {
                oldPix = effect->apply(iconPix, KIconLoader::Desktop, KIconLoader::ActiveState);
            }
        } else {
            oldPix = effect->apply(iconPix, KIconLoader::Desktop, KIconLoader::DisabledState);
        }
        iconPix = Plasma::PaintUtils::transition(oldPix, iconPix, s->animationProgress());
    }
    painter->drawPixmap(0, 0, iconPix);
}

void AuroraeMenuButton::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    emit doubleClicked();
}

void AuroraeMenuButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
    if (isPressed()) {
        if (m_timer->isActive()) {
            m_timer->stop();
        } else {
            m_timer->start();
        }
    }
    setPressed(false);
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void AuroraeMenuButton::setIcon(const QPixmap& icon)
{
    m_icon = icon;
    update();
}

/************************************************
* AuroraeSpacer
************************************************/
AuroraeSpacer::AuroraeSpacer(AuroraeTheme *theme)
    : QGraphicsWidget()
    , m_theme(theme)
{
}

QSizeF AuroraeSpacer::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which)
    Q_UNUSED(constraint)
    return QSizeF(m_theme->themeConfig().explicitButtonSpacer(),
                  m_theme->themeConfig().buttonHeight());
}

} // namespace
