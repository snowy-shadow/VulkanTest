target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		Event.cppm
		
		EventBase.cppm
		MouseEvent.cppm
		ApplicationEvent.cppm
		KeyEvent.cppm
)
