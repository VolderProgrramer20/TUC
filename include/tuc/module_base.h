// Х... типо смешно хаха ибо кто-то решил добавить новый файл сюда спустя год. Шутку поймут только единицы. (Автор коммита не считае-)
// И вообще, а почему репозиторий не архивирован?

class shell;

class module_base {
	public:
		module_base(shell* core);
		virtual void onLoad() = 0;
		virtual void onUnload() = 0;
	private:
		shell* _corePtr;
};
