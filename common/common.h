#ifndef COMMON_H
#define COMMON_H
#define CLASS1_STR "班级一"
#define CLASS2_STR "班级二"

// 定义服务器状态枚举（清晰区分不同状态）
enum SERVER_STATE {
    SERVER_STATE_STOPPED, // 已停止
    SERVER_STATE_RUNNING  // 运行中（监听中）
};
#endif // COMMON_H
