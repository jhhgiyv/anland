# Anland Consumer 状态 D-Bus 接口

Anland 的 KWin backend 在 Linux Session Bus 上导出 Android consumer 的连接状态。其他程序可以订阅该接口，以判断 Android 上是否存在正在消费 Anland 输出的显示窗口。

## 状态含义

`Active` 为 `true` 表示 Android consumer 已完成资源握手，并正在消费 Anland 输出。

`Active` 为 `false` 表示当前没有可用 consumer，例如 Android 显示窗口进入后台、关闭、连接断开，或 backend 正在等待重连。

该状态不表示 Android 窗口是否拥有输入焦点，也不表示 SettingsActivity 是否可见。

## D-Bus 契约

| 项目 | 值 |
| --- | --- |
| Bus | Session Bus |
| 服务名 | `org.anland.Consumer` |
| 对象路径 | `/org/anland/Consumer` |
| 接口名 | `org.anland.Consumer` |
| 只读属性 | `Active`，类型 `b` (`bool`) |
| 状态变更信号 | `ActiveChanged(b active)` |

服务随 KWin 的 Anland backend 生命周期存在。没有该服务通常表示 KWin 未以 Anland backend 运行、backend 初始化失败，或接入程序连接到了错误的 Session Bus。

## 使用方式

接入程序必须运行在启动 KWin 的同一 Linux Session Bus 中。项目启动脚本使用 `dbus-run-session` 启动 Plasma，因此从其他终端连接时需继承同一个 `DBUS_SESSION_BUS_ADDRESS`。

推荐顺序：先订阅 `ActiveChanged`，再读取 `Active` 属性，并以最后收到的信号或读取结果维护本地状态。这样可在订阅和首次查询相邻的状态变化中保持正确结果。

### 查询当前状态

```sh
busctl --user get-property \
  org.anland.Consumer \
  /org/anland/Consumer \
  org.anland.Consumer \
  Active
```

输出 `b true` 表示 consumer 正在消费输出；`b false` 表示未连接。

等价的 `gdbus` 命令：

```sh
gdbus call --session \
  --dest org.anland.Consumer \
  --object-path /org/anland/Consumer \
  --method org.freedesktop.DBus.Properties.Get \
  org.anland.Consumer Active
```

### 监听状态变化

```sh
gdbus monitor --session \
  --dest org.anland.Consumer \
  --object-path /org/anland/Consumer
```

状态变化时会收到类似输出：

```text
/org/anland/Consumer: org.anland.Consumer.ActiveChanged (true,)
/org/anland/Consumer: org.anland.Consumer.ActiveChanged (false,)
```

### Qt/C++ 示例

```cpp
auto bus = QDBusConnection::sessionBus();
bus.connect(QStringLiteral("org.anland.Consumer"),
            QStringLiteral("/org/anland/Consumer"),
            QStringLiteral("org.anland.Consumer"),
            QStringLiteral("ActiveChanged"),
            receiver, SLOT(onAnlandConsumerActiveChanged(bool)));

QDBusInterface status(QStringLiteral("org.anland.Consumer"),
                      QStringLiteral("/org/anland/Consumer"),
                      QStringLiteral("org.anland.Consumer"), bus);
const bool active = status.property("Active").toBool();
```

`onAnlandConsumerActiveChanged(bool active)` 应更新接入程序中的状态。属性读取或信号订阅失败时，应将服务不可用作为独立状态处理，而不是当作 `Active == false`。

## 行为保证

- `ActiveChanged` 仅在布尔状态实际变化时发送，不会因重连轮询重复发送。
- consumer 成功完成资源握手后发送 `ActiveChanged(true)`。
- consumer 连接丢失后发送 `ActiveChanged(false)`。
- KWin/backend 退出时服务名消失；订阅方可同时监听 `org.freedesktop.DBus.NameOwnerChanged` 处理服务退出和重新出现。
