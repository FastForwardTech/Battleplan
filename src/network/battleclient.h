/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef BattleClient_H
#define BattleClient_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include "network/state.h"
#include "player.h"

class BattleClient : public QObject
{
    Q_OBJECT
public:
	explicit BattleClient(QObject *parent = nullptr);
	~BattleClient();

	void initializeState(State::GameState aState);
	// same thing as initializeState without sending over network
	void updateLocalState(State::GameState aState);
	void connectToServer(const QUrl &url);
	void disconnect();

	void initializeMap(QByteArray mapData);

	State::GameState getState() const;

Q_SIGNALS:
	void closed();
	void connected();

	void mapUpdateFromServer(QImage img);
	void stateUpdateFromServer(State::GameState newState);

public Q_SLOTS:
	void updateGridOffsetX(int aOffset);
	void updateGridOffsetY(int aOffset);
	void updateGridStep(qreal aStep);
	void updatePlayers(QVector<Player*> aPlayers);

private Q_SLOTS:
    void onConnected();
	void onBinaryMessageReceived(QByteArray data);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
	State::GameState state;
	void sendState();

	enum MsgType {
		MAP,
		STATE
	};

	struct BattleMessage {
		MsgType type;
		QByteArray data;

		QByteArray serialize()
		{
			QByteArray byteArray;

			QDataStream stream(&byteArray, QIODevice::WriteOnly);

			stream << type;
			stream << data;

			return byteArray;
		}

		BattleMessage deserialize(const QByteArray& byteArray)
		{
			QDataStream stream(byteArray);
			BattleMessage msg;

			stream >> msg.type
					>> msg.data;
			return msg;
		}

	};

	void sendBattleMessage(BattleMessage msg);
};

#endif // BattleClient_H
