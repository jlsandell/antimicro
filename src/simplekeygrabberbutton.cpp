#include "simplekeygrabberbutton.h"
#include "event.h"
#include "wininfo.h"

SimpleKeyGrabberButton::SimpleKeyGrabberButton(QWidget *parent) :
    QPushButton(parent)
{
    grabNextAction = false;
    grabbingWheel = false;
    this->installEventFilter(this);
}

SimpleKeyGrabberButton::SimpleKeyGrabberButton(int controlcode, JoyButtonSlot::JoySlotInputAction mode, QWidget *parent) :
    QPushButton(parent)
{
    grabNextAction = false;
    grabbingWheel = false;
    setValue(controlcode, mode);
}

void SimpleKeyGrabberButton::keyPressEvent(QKeyEvent *event)
{
    // Do not allow closing of dialog using Escape key
    if (event->key() == Qt::Key_Escape)
    {
        return;
    }

    QPushButton::keyPressEvent(event);
}

bool SimpleKeyGrabberButton::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    int controlcode = 0;
    if (grabNextAction && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEve = (QMouseEvent*) event;
        if (mouseEve->button() == Qt::RightButton)
        {
            controlcode = 3;
        }
        else if (mouseEve->button() == Qt::MiddleButton)
        {
            controlcode = 2;
        }
        else {
            controlcode = mouseEve->button();
        }

        setText(QString(tr("Mouse")).append(" ").append(QString::number(controlcode)));

        buttonslot.setSlotCode(controlcode);
        buttonslot.setSlotMode(JoyButtonSlot::JoyMouseButton);
        releaseMouse();
        releaseKeyboard();

        grabNextAction = grabbingWheel = false;
        emit buttonCodeChanged(controlcode);
    }
    else if (grabNextAction && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEve = (QKeyEvent*) event;
        int tempcode = keyEve->nativeScanCode();
        int virtualactual = keyEve->nativeVirtualKey();

#ifndef Q_OS_WIN
        Q_UNUSED(virtualactual);
#endif

        if ((keyEve->modifiers() & Qt::ControlModifier) && keyEve->key() == Qt::Key_X)
        {
            controlcode = 0;
            setText("");
        }
        else if (controlcode < 0)
        {
            controlcode = 0;
            setText("");
        }
        else
        {
#if defined(Q_OS_UNIX)
            controlcode = tempcode;
            buttonslot.setSlotCode(controlcode);
            buttonslot.setSlotMode(JoyButtonSlot::JoyKeyboard);
            setText(keycodeToKey(controlcode).toUpper());

#elif defined(Q_OS_WIN)
            controlcode = WinInfo::correctVirtualKey(tempcode, virtualactual);
            buttonslot.setSlotCode(controlcode);
            buttonslot.setSlotMode(JoyButtonSlot::JoyKeyboard);
            setText(keycodeToKey(controlcode).toUpper());

#endif
        }

        grabNextAction = false;
        grabbingWheel = false;
        releaseMouse();
        releaseKeyboard();

        emit buttonCodeChanged(controlcode);
    }
    else if (grabNextAction && event->type() == QEvent::Wheel && !grabbingWheel)
    {
        grabbingWheel = true;
    }
    else if (grabNextAction && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEve = (QWheelEvent*) event;
        QString text = QString(tr("Mouse")).append(" ");

        if (wheelEve->orientation() == Qt::Vertical && wheelEve->delta() >= 120)
        {
            controlcode = 4;
        }
        else if (wheelEve->orientation() == Qt::Vertical && wheelEve->delta() <= -120)
        {
            controlcode = 5;
        }
        else if (wheelEve->orientation() == Qt::Horizontal && wheelEve->delta() >= 120)
        {
            controlcode = 6;
        }
        else if (wheelEve->orientation() == Qt::Horizontal && wheelEve->delta() <= -120)
        {
            controlcode = 7;
        }

        if (controlcode > 0)
        {
            text = text.append(QString::number(controlcode));
            setText(text);

            grabNextAction = false;
            grabbingWheel = false;
            releaseMouse();
            releaseKeyboard();
            buttonslot.setSlotCode(controlcode);
            buttonslot.setSlotMode(JoyButtonSlot::JoyMouseButton);
            emit buttonCodeChanged(controlcode);
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEve = (QMouseEvent*) event;
        if (mouseEve->button() == Qt::LeftButton)
        {
            grabNextAction = true;
            setText("...");
            setFocus();
            grabKeyboard();
            grabMouse();
        }
    }


    return false;
}

void SimpleKeyGrabberButton::setValue(int value, JoyButtonSlot::JoySlotInputAction mode)
{
    buttonslot.setSlotCode(value);
    buttonslot.setSlotMode(mode);

    setText(buttonslot.getSlotString());
}

JoyButtonSlot* SimpleKeyGrabberButton::getValue()
{
    return &buttonslot;
}
