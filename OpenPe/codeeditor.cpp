/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/



#include "codeeditor.h"
#include <Windows.h>
#include <QScopedPointer>
#include "highlighter.h"
 #include <QtGui>


 CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
 {
     lineNumberArea = new LineNumberArea(this);

     connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
     connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
     connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

     updateLineNumberAreaWidth(0);
     highlightCurrentLine();
	 Highlighter* highlighter = new Highlighter(this->document());

	 flowStruc= new QMenu();  
	 flowStruc->setTitle("Control Structures");
	 flowStruc->addAction("Loop: For",this,SLOT(flowStrucSlot()),0);	   
	 flowStruc->addAction("Loop: While",this,SLOT(flowStrucSlot()),0);	   
	 flowStruc->addAction("Loop: Do...While",this,SLOT(flowStrucSlot()),0);	   
	 flowStruc->addAction("Flow control: if",this,SLOT(flowStrucSlot()),0);	   
	 flowStruc->addAction("Flow control: if...else",this,SLOT(flowStrucSlot()),0);
	 flowStruc->addAction("Time control: Sleep",this,SLOT(flowStrucSlot()),0);

	 customFuns = new QMenu();
	 customFuns->setTitle("Packet Functions");
	 customFuns->addAction("SendPacket: Single Packet",this,SLOT(customFunsSlot()),0);
	 customFuns->addAction("SendPacket: Single Packet and Sleep after",this,SLOT(customFunsSlot()),0);
	 customFuns->addAction("SendPacket: Single Packet, Sleep after and repeat",this,SLOT(customFunsSlot()),0);
	 customFuns->addSeparator();
	 customFuns->addAction("RecvPacket: Single Packet",this,SLOT(customFunsSlot()),0);
	 customFuns->addAction("RecvPacket: Single Packet and Sleep after",this,SLOT(customFunsSlot()),0);
	 customFuns->addAction("RecvPacket: Single Packet, Sleep after and repeat",this,SLOT(customFunsSlot()),0);

	 blacklistFunc = new QMenu();
	 blacklistFunc->setTitle("BlackList Control");
	 blacklistFunc->addAction("Packet Flow: Block Packet",this,SLOT(blacklistfuncSlot()),0);
	 blacklistFunc->addAction("Packet Flow: Unblock Packet",this,SLOT(blacklistfuncSlot()),0);
	 blacklistFunc->addAction("Packet Flow: Unblock all blocked Packets",this,SLOT(blacklistfuncSlot()),0);

	 currPkt= new QMenu();
	 currPkt->setTitle("Current Packet Control");
	 currPkt->addAction("Block",this,SLOT(currPktSlot()),0);
	 currPkt->addAction("Check for Pattern",this,SLOT(currPktSlot()),0);
	 currPkt->addAction("Modify",this,SLOT(currPktSlot()),0);
	 currPkt->addSeparator();
	 currPkt->addAction("Size",this,SLOT(currPktSlot()),0);
	 currPkt->addAction("ReadByte",this,SLOT(currPktSlot()),0);
	 currPkt->addAction("ReadWord",this,SLOT(currPktSlot()),0);
	 currPkt->addAction("ReadDWord",this,SLOT(currPktSlot()),0);


	 mapleFunc = new QMenu();
	 mapleFunc->setTitle("Memory Functions");
//	 mapleFunc->addAction("Fix HP/MP",this,SLOT(mapleFuncSlot()),0);
	 mapleFunc->addAction("ReadPointer",this,SLOT(mapleFuncSlot()),0);
	 mapleFunc->addAction("WritePointer",this,SLOT(mapleFuncSlot()),0);
//	 mapleFunc->addAction("Get current HP",this,SLOT(mapleFuncSlot()),0);
//	 mapleFunc->addAction("Get current MP",this,SLOT(mapleFuncSlot()),0);
//	 mapleFunc->addAction("Get current People",this,SLOT(mapleFuncSlot()),0);

	 counterFunc = new QMenu();
	 counterFunc->setTitle("Counters Control");
	 counterFunc->addAction("Create new Counter",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Delete Counter",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Get Value of a Counter",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Increase a Counter",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Decrease a Counter",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Set a Counter value",this,SLOT(counterSlot()),0);
	 counterFunc->addAction("Reset a Counter (set value to 0)",this,SLOT(counterSlot()),0);
	 
	 flagsFunc = new QMenu();
	 flagsFunc->setTitle("Flags Control");
	 flagsFunc->addAction("Create new Flag",this,SLOT(flagsSlot()),0);
	 flagsFunc->addAction("Delete Flag",this,SLOT(flagsSlot()),0);
	 flagsFunc->addAction("Get status of a Flag",this,SLOT(flagsSlot()),0);
	 flagsFunc->addAction("Set status of a Flag",this,SLOT(flagsSlot()),0);
	 flagsFunc->addAction("Toggle Flag",this,SLOT(flagsSlot()),0);

	 converterFunc = new QMenu();
	 converterFunc->setTitle("String Converter");
	 converterFunc->addAction("ToHexString",this,SLOT(converterSlot()),0);
	 converterFunc->addAction("ToMapleString",this,SLOT(converterSlot()),0);
	 converterFunc->addAction("ToLittleEndian",this,SLOT(converterSlot()),0);
	  converterFunc->addAction("Format",this,SLOT(converterSlot()),0);


	 GlobalMenu = new QMenu(this);
	 GlobalMenu->addMenu(flowStruc);
	 GlobalMenu->addSeparator();
	  GlobalMenu->addMenu(currPkt);
	 GlobalMenu->addMenu(customFuns);
	GlobalMenu->addSeparator();
	 GlobalMenu->addMenu(blacklistFunc);	 
	 GlobalMenu->addSeparator();
	 GlobalMenu->addMenu(counterFunc);
	 GlobalMenu->addMenu(flagsFunc);
	 GlobalMenu->addMenu(mapleFunc);
	 GlobalMenu->addMenu(converterFunc);
	 GlobalMenu->addAction("ShowMessage",this,SLOT(flagsSlot()),0);
 }



 int CodeEditor::lineNumberAreaWidth()
 {
     int digits = 1;
     int max = qMax(1, blockCount());
     while (max >= 10) {
         max /= 10;
         ++digits;
     }

     int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

     return space;
 }



 void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
 {
     setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
 }



 void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
 {
     if (dy)
         lineNumberArea->scroll(0, dy);
     else
         lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

     if (rect.contains(viewport()->rect()))
         updateLineNumberAreaWidth(0);
 }



 void CodeEditor::resizeEvent(QResizeEvent *e)
 {
     QPlainTextEdit::resizeEvent(e);

     QRect cr = contentsRect();
     lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
 }



 void CodeEditor::highlightCurrentLine()
 {
     QList<QTextEdit::ExtraSelection> extraSelections;

     if (!isReadOnly()) {
         QTextEdit::ExtraSelection selection;

         QColor lineColor = QColor(0, 0, 255,50);

         selection.format.setBackground(lineColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);
         selection.cursor = textCursor();
         selection.cursor.clearSelection();
         extraSelections.append(selection);
     }

     setExtraSelections(extraSelections);
 }



 void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
 {
     QPainter painter(lineNumberArea);
     painter.fillRect(event->rect(), Qt::lightGray);


     QTextBlock block = firstVisibleBlock();
     int blockNumber = block.blockNumber();
     int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
     int bottom = top + (int) blockBoundingRect(block).height();

     while (block.isValid() && top <= event->rect().bottom()) {
         if (block.isVisible() && bottom >= event->rect().top()) {
             QString number = QString::number(blockNumber + 1);
             painter.setPen(Qt::black);
             painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                              Qt::AlignRight, number);
         }

         block = block.next();
         top = bottom;
         bottom = top + (int) blockBoundingRect(block).height();
         ++blockNumber;
     }
 }

 
 
 
  void CodeEditor::contextMenuEvent ( QContextMenuEvent * event )
  {

     GlobalMenu->exec(QCursor::pos());
  }

  void CodeEditor::flowStrucSlot()
  {
	  QObject* pSender = sender();
	  if(pSender == flowStruc->actions()[0])
	  {
    QString& ini = QInputDialog::getText(this, "Initialization", "Please enter a variable initialization.");
	QString& cond = QInputDialog::getText(this, "End Condition", "Please enter an end condition.");
	QString& incre = QInputDialog::getText(this, "Increase statement", "Please enter an increase statement.");
	this->insertPlainText("for("+ini+";"+cond+";"+incre+")\n{\n//To do code here...\n}");
	  }

	  if(pSender == flowStruc->actions()[1])
	  {
    QString& cond = QInputDialog::getText(this, "End Condition", "Please enter an end condition.");
	this->insertPlainText("while("+cond+")\n{\n//To do code here...\n}");
	  }

	   if(pSender == flowStruc->actions()[2])
	  {
    QString& cond = QInputDialog::getText(this, "End Condition", "Please enter an end condition.");
	this->insertPlainText("do\n{\n//To do code here...\n} while("+cond+")");
	  }

	   if(pSender == flowStruc->actions()[3])
	  {
    QString& cond = QInputDialog::getText(this, "End Condition", "Please enter an end condition.");
	this->insertPlainText("if("+cond+")\n{\n//To do code here...\n}");
	  }

	   	   if(pSender == flowStruc->actions()[4])
	  {
    QString& cond = QInputDialog::getText(this, "End Condition", "Please enter an end condition.");
	this->insertPlainText("if("+cond+")\n{\n//To do code if true here...\n}\nelse\n{\n//To do code if false here...\n}");
	  }
		   QString time;
		   bool fSucceeded;
		  if(pSender == flowStruc->actions()[5])
	  {
		    do{
		   time = QInputDialog::getText(this, "Sleep time", "Please enter a time to sleep on ms.");
		   WORD wHeader = time.toUShort(&fSucceeded, 10);
		   }while(!fSucceeded);
		   this->insertPlainText("Sleep("+time+");");
	}
  }

  void CodeEditor::customFunsSlot()
  {
	  QObject* pSender = sender();
	  QString aux;
	  QString pkt,time,repeat;
	  bool fSucceeded;
	  if(pSender == customFuns->actions()[0])
	  { 
		   pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to send.");		     
		   this->insertPlainText("SendPacket(\""+pkt+"\");");
	  }
	  if(pSender == customFuns->actions()[1])
	  {
		  WORD wHeader;		   
		   pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to send.");		   
		   do{
		   time = QInputDialog::getText(this, "Sleep time", "Please enter a time to sleep on ms.");
		   wHeader = time.toUShort(&fSucceeded, 10);
		   }while(!fSucceeded);
		   this->insertPlainText("SendPacket(\""+pkt+"\","+time+");");
	  }

	  if(pSender == customFuns->actions()[2])
	  {
		   WORD wHeader;		   
		   pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to send.");		 
		   do{
		   time = QInputDialog::getText(this, "Sleep time", "Please enter a time to sleep on ms.");
		   wHeader = time.toUShort(&fSucceeded, 10);
		   }while(!fSucceeded);
		   do{
		   repeat = QInputDialog::getText(this, "Repeat", "Please enter the number of time to send the packet.");
		   wHeader = repeat.toUShort(&fSucceeded, 10);
		   }while(!fSucceeded);
		   this->insertPlainText("SendPacket(\""+pkt+"\","+time+","+repeat+");");
	  }

	

	  if(pSender == customFuns->actions()[4])
	  { 
		  pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to recv.");		     
		  this->insertPlainText("RecvPacket(\""+pkt+"\");");
	  }
	  if(pSender == customFuns->actions()[5])
	  {
		  WORD wHeader;		   
		  pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to recv.");		   
		  do{
			  time = QInputDialog::getText(this, "Sleep time", "Please enter a time to sleep on ms.");
			  wHeader = time.toUShort(&fSucceeded, 10);
		  }while(!fSucceeded);
		  this->insertPlainText("RecvPacket(\""+pkt+"\","+time+");");
	  }

	  if(pSender == customFuns->actions()[6])
	  {
		  WORD wHeader;		   
		  pkt = QInputDialog::getText(this, "Packet", "Please enter a valid Packet to recv.");		 
		  do{
			  time = QInputDialog::getText(this, "Sleep time", "Please enter a time to sleep on ms.");
			  wHeader = time.toUShort(&fSucceeded, 10);
		  }while(!fSucceeded);
		  do{
			  repeat = QInputDialog::getText(this, "Repeat", "Please enter the number of time to send the packet.");
			  wHeader = repeat.toUShort(&fSucceeded, 10);
		  }while(!fSucceeded);
		  this->insertPlainText("RecvPacket(\""+pkt+"\","+time+","+repeat+");");
	  }
  }

 void CodeEditor::blacklistfuncSlot()
 {
	 QObject* pSender = sender();
	 QString aux;
	 QString pkt,time,repeat;
	 bool fSucceeded;
	 if(pSender == blacklistFunc->actions()[0])
	 {
		 QScopedPointer<QMessageBox> mb(new QMessageBox(this));
		 QScopedPointer<QPushButton> pb1 (mb->addButton("Out", QMessageBox::ButtonRole::ActionRole));
		 QScopedPointer<QPushButton> pb2 (mb->addButton("In", QMessageBox::ButtonRole::ActionRole));
		 mb->setWindowTitle("Packet Type");
		 mb->setText("Please select the packet type (In or Out).");
		 mb->setIcon(QMessageBox::Icon::Information);
		 mb->exec();
		 bool type = (mb->clickedButton() == pb1.take() ? true : false);
		 QString stype = type == type ? "In" : "Out";
		 pkt = QInputDialog::getText(this, "Header", "Please enter a valid Header to block.");
		 this->insertPlainText("BlackList.Block(\""+stype+"\",0x"+pkt+");");
	 }

	 if(pSender == blacklistFunc->actions()[1])
	 {
		 QScopedPointer<QMessageBox> mb(new QMessageBox(this));
		 QScopedPointer<QPushButton> pb1 (mb->addButton("Out", QMessageBox::ButtonRole::ActionRole));
		 QScopedPointer<QPushButton> pb2 (mb->addButton("In", QMessageBox::ButtonRole::ActionRole));
		 mb->setWindowTitle("Packet Type");
		 mb->setText("Please select the packet type (In or Out).");
		 mb->setIcon(QMessageBox::Icon::Information);
		 mb->exec();

		 bool type = (mb->clickedButton() == pb1.take() ? true : false);
		 QString stype = type == type ? "In" : "Out";


		 pkt = QInputDialog::getText(this, "Header", "Please enter a valid Header to unblock.");


		 this->insertPlainText("BlackList.UnBlock(\""+stype+"\",0x"+pkt+");");
	 }

	 if(pSender == blacklistFunc->actions()[2])
	 {
		 this->insertPlainText("BlackList.UnBlockAll();");
	 }
 }
void CodeEditor::mapleFuncSlot()
{
	QObject* pSender = sender();
	QString aux;
	QString base, offset, value;
	  if(pSender == mapleFunc->actions()[0])
	  { 
      base = QInputDialog::getText(this, "Pointer base", "Please the pointer base:(0x********).");
	  offset = QInputDialog::getText(this, "Pointer offset", "Please the pointer offset:(0x********).");
	  this->insertPlainText("ReadPointer(\""+base+"\",\""+offset+"\");");
	   }

	   if(pSender == mapleFunc->actions()[1])
	   {
		   base = QInputDialog::getText(this, "Pointer base", "Please the pointer base:(0x********).");
		   offset = QInputDialog::getText(this, "Pointer offset", "Please the pointer offset:(0x********).");
		   value = QInputDialog::getText(this, "Value", "Please the value.");
		   this->insertPlainText("WritePointer("+base+","+offset+","+value+");");
	   }

	 
}

void CodeEditor::currPktSlot()
{
	QObject* pSender = sender();
	QString aux, patt;
	QString pkt,time,repeat;
	bool fSucceeded;
	int offset;
	if(pSender == currPkt->actions()[0])//block
	{
		this->insertPlainText("ThisPacket.Block();");
	}
	if(pSender == currPkt->actions()[1])//Check for Pattern
	{
		patt = QInputDialog::getText(this, "Pattern to check", "Please enter a valid pattern.\nUse ? to avoid checking one byte.\nUse '...' to avoid checking last part of the packet.");
		this->insertPlainText("ThisPacket.CheckPattern(\""+patt+"\");");
	}
	if(pSender == currPkt->actions()[2])//Modify
	{
		patt = QInputDialog::getText(this, "Patten to output the packet", "Please enter a valid pattern.\nUse ? to avoid modifying of one byte.\nUse '...' to avoid modifying last part of the packet (if you don't use this, packet will be cut off).");
		this->insertPlainText("ThisPacket.Mod(\""+patt+"\");");
	}
	if(pSender == currPkt->actions()[4])//Size
	{
		this->insertPlainText("ThisPacket.Size();");
	}
	if(pSender == currPkt->actions()[5])//ReadByte
	{
		do{
			time = QInputDialog::getText(this, "Offset", "Please enter the position of byte that u want to read.");
			offset = time.toUShort(&fSucceeded, 10);
		}while(!fSucceeded);
		this->insertPlainText("ThisPacket.ReadByte("+time+");");
	}
	if(pSender == currPkt->actions()[6])//ReadWord
	{
		do{
			time = QInputDialog::getText(this, "Offset", "Please enter the position of word that u want to read.");
			offset = time.toUShort(&fSucceeded, 10);
		}while(!fSucceeded);
		this->insertPlainText("ThisPacket.ReadWord("+time+");");
	}
	if(pSender == currPkt->actions()[7])//ReadDWord
	{
		do{
			time = QInputDialog::getText(this, "Offset", "Please enter the position of dword that u want to read.");
			offset = time.toUShort(&fSucceeded, 10);
		}while(!fSucceeded);
		this->insertPlainText("ThisPacket.ReadDWord("+time+");");
	}
}

void CodeEditor::counterSlot()
{
	QObject* pSender = sender();
	QString aux, patt;
	QString pkt,time,repeat;
	bool fSucceeded;
	int offset;

	if(pSender == counterFunc->actions()[0])//Create new Counter
	{
		patt = QInputDialog::getText(this, "Name of the new Counter", "Please enter the name under which this counter will be referenced.");
		aux = QInputDialog::getText(this, "Initial value of this Counter", "Please enter a initial value for this counter.\nYou can leave this empty if you want the default value (0).");
		if (aux.isEmpty())
		{
			this->insertPlainText("Counters.Create(\""+patt+"\");");
		}
		else
		{
			this->insertPlainText("Counters.Create(\""+patt+"\","+aux+");");
		}
		
	}
	if(pSender == counterFunc->actions()[1])//Delete a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to delete.");
		this->insertPlainText("Counters.Delete(\""+patt+"\");");
	}
	if(pSender == counterFunc->actions()[2])//Get Value of a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to know the value.");
		this->insertPlainText("Counters.GetValue(\""+patt+"\");");
	}
	if(pSender == counterFunc->actions()[3])//Increase a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to Increase the value.");
		this->insertPlainText("Counters.Inc(\""+patt+"\");");
	}
	if(pSender == counterFunc->actions()[4])//Decrease a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to Decrease the value.");
		this->insertPlainText("Counters.Dec(\""+patt+"\");");
	}
	if(pSender == counterFunc->actions()[5])//Set a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to set the value.");
		aux = QInputDialog::getText(this, "Value for this Counter", "Please enter a value for this counter.");

		this->insertPlainText("Counters.Set(\""+patt+"\","+aux+");");
	}
	if(pSender == counterFunc->actions()[6])//Reset a Counter
	{
		patt = QInputDialog::getText(this, "Name of the Counter", "Please enter the the name of the counter that you want to Reset the value.");
		this->insertPlainText("Counters.Reset(\""+patt+"\");");
	}

}

void CodeEditor::flagsSlot()
{
	QObject* pSender = sender();
	QString aux, patt;
	QString pkt,time,repeat;
	bool fSucceeded;
	int offset;

if(pSender == counterFunc->actions()[0])//Create new flag
{
	patt = QInputDialog::getText(this, "Name of the new Flag", "Please enter the name under which this flag will be referenced.");
	aux = QInputDialog::getText(this, "Initial value of this Flag", "Please enter a initial value for this flag.\nYou can leave this empty if you want the default value (false).");
	if (aux.isEmpty())
	{
		this->insertPlainText("Flags.Create(\""+patt+"\");");
	}
	else
	{
		this->insertPlainText("Flags.Create(\""+patt+"\","+aux+");");
	}
}
else if(pSender == counterFunc->actions()[1])//Delete
{
	patt = QInputDialog::getText(this, "Name of the Flag", "Please enter the name of the flag that you want to delete.");
	this->insertPlainText("Flags.Delete(\""+patt+"\");");
}
else if(pSender == counterFunc->actions()[2])//Get status
{
	patt = QInputDialog::getText(this, "Name of the Flag", "Please enter the name of the flag that you want to know the status.");
	this->insertPlainText("Flags.GetStatus(\""+patt+"\");");
}
else if(pSender == counterFunc->actions()[3])//Set status
{
	patt = QInputDialog::getText(this, "Name of the Flag", "Please enter the name of the flag that you want to set the status.");
	aux = QInputDialog::getText(this, "Value of this Flag", "Please enter the new status for this flag.");
	this->insertPlainText("Flags.SetStatus(\""+patt+"\","+aux+");");
}
else if(pSender == counterFunc->actions()[4])//Toggle
{
	patt = QInputDialog::getText(this, "Name of the Flag", "Please enter the name of the flag that you want to toggle the status.");
	this->insertPlainText("Flags.Toggle(\""+patt+"\");");
}
else
{
	patt = QInputDialog::getText(this, "Title", "Please enter a title for your message.");
	aux = QInputDialog::getText(this, "Message", "Please enter your message.");

	this->insertPlainText("ShowMessage(\""+patt+"\",\""+aux+"\");");
}
}


void CodeEditor::converterSlot()
{
	QObject* pSender = sender();
	QString aux, patt;
	QString pkt,time,repeat;
	bool fSucceeded;
	int offset;
	if(pSender == converterFunc->actions()[0])//ToHexString
	{
		patt = QInputDialog::getText(this, "Value", "Please enter the value you want to convert.");

		QScopedPointer<QMessageBox> mb(new QMessageBox(this));
		QScopedPointer<QPushButton> pb1 (mb->addButton("Byte", QMessageBox::ButtonRole::ActionRole));
		QScopedPointer<QPushButton> pb2 (mb->addButton("Word", QMessageBox::ButtonRole::ActionRole));
		QScopedPointer<QPushButton> pb3 (mb->addButton("Dword", QMessageBox::ButtonRole::ActionRole));
		mb->setWindowTitle("Output type");
		mb->setText("Please select the output type:");
		mb->setIcon(QMessageBox::Icon::Question);
		mb->exec();

		aux = mb->clickedButton()->text();

		this->insertPlainText("Converter.ToHexString("+patt+",\""+aux+"\");");
	}
	if(pSender == converterFunc->actions()[1])//ToMapleString
	{
		patt = QInputDialog::getText(this, "Text", "Please enter the text you want to convert.");
		this->insertPlainText("Converter.ToMapleString(\""+patt+"\");");
	}
	if(pSender == converterFunc->actions()[2])//ToLittleEndian"
	{
		patt = QInputDialog::getText(this, "Text", "Please enter the text you want to convert.");
		this->insertPlainText("Converter.ToLittleEndian(\""+patt+"\");");
	}
	if(pSender == converterFunc->actions()[3])//Format
	{
		patt = QInputDialog::getText(this, "Format", "Please enter the text you want to format.\nYou need to enter the arguments manually after this!");
		this->insertPlainText("Format(\""+patt+"\",/*Args Here*/);");
	}
}