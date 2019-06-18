chat_ROOT="$(dirname "$PWD")"
chat_ROOT="$(dirname "$chat_ROOT")"
chat_ROOT="$(dirname "$chat_ROOT")"/out
LD_LIBRARY_PATH=$chat_ROOT/lib $chat_ROOT/bin/chat --config-file=user.conf
