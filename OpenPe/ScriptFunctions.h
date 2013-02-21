#include <QtScript>

QScriptValue ShowMessage(QScriptContext *context, QScriptEngine *engine);

QScriptValue ScriptPktSend(QScriptContext *context, QScriptEngine *engine);
QScriptValue ScriptPktRecv(QScriptContext *context, QScriptEngine *engine);
QScriptValue ScriptSleep(QScriptContext *context, QScriptEngine *engine);

QScriptValue ScriptFormat(QScriptContext *context, QScriptEngine *engine);

QScriptValue ScriptReadPointer(QScriptContext *context, QScriptEngine *engine);
QScriptValue ScriptWritePointer(QScriptContext *context, QScriptEngine *engine);
QScriptValue ScriptDoLater(QScriptContext *context, QScriptEngine *engine);
