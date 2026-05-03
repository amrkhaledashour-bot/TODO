cmake_minimum_required(VERSION 3.16)

project(NotionTodo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Concurrent)

set(SOURCES
    src/main.cpp
    
    # Models
    src/models/User.h
    src/models/Workspace.h
    src/models/Task.h
    src/models/JournalEntry.h
    src/models/Tag.h

    # Database
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/database/AsyncExecutor.h

    # Utils
    src/utils/PasswordHasher.h
    src/utils/PasswordHasher.cpp
    src/utils/SessionManager.h
    src/utils/SessionManager.cpp

    # Controllers
    src/controllers/AuthController.h
    src/controllers/AuthController.cpp
    src/controllers/WorkspaceController.h
    src/controllers/WorkspaceController.cpp
    src/controllers/TaskController.h
    src/controllers/TaskController.cpp
    src/controllers/JournalController.h
    src/controllers/JournalController.cpp

    # Widgets
    src/widgets/ThemeManager.h
    src/widgets/ThemeManager.cpp
    src/widgets/TaskCard.h
    src/widgets/TaskCard.cpp
    src/widgets/TaskColumn.h
    src/widgets/TaskColumn.cpp
    src/widgets/SidebarWidget.h
    src/widgets/SidebarWidget.cpp

    # Views
    src/views/LoginView.h
    src/views/LoginView.cpp
    src/views/RegisterView.h
    src/views/RegisterView.cpp
    src/views/DashboardView.h
    src/views/DashboardView.cpp
    src/views/MainView.h
    src/views/MainView.cpp
    src/views/JournalView.h
    src/views/JournalView.cpp
    src/views/TaskEditorDialog.h
    src/views/TaskEditorDialog.cpp
    src/views/MainWindow.h
    src/views/MainWindow.cpp

    # Resources
    resources/resources.qrc
)

add_executable(NotionTodo ${SOURCES})

target_link_libraries(NotionTodo PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Sql
    Qt6::Concurrent
)

# Set Windows executable properties
if(WIN32)
    set_target_properties(NotionTodo PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()
