#include <wx/wx.h>
#include <wx/listbox.h>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>

class TodoApp : public wxApp {
public:
    virtual bool OnInit() override;
};

class TodoFrame : public wxFrame {
public:
    TodoFrame(const wxString& title);

private:
    void OnAdd(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void LoadTasks();
    void SaveTasks();
    bool IsAppRunning();

    wxTextCtrl* m_taskInput;
    wxButton* m_addButton;
    wxListBox* m_taskList;

    std::vector<std::string> tasks;
    const std::string taskFile = "/home/user/todolist.txt";  // Путь к файлу для хранения задач, адаптируйте под вашу систему

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(TodoFrame, wxFrame)
EVT_BUTTON(1001, TodoFrame::OnAdd)
EVT_BUTTON(1002, TodoFrame::OnDelete)
EVT_MENU(wxID_EXIT, TodoFrame::OnExit)
wxEND_EVENT_TABLE()

bool TodoApp::OnInit() {
    TodoFrame* frame = new TodoFrame("Список дел");
    frame->Show(true);
    return true;
}

TodoFrame::TodoFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)) {
    if (IsAppRunning()) {
        wxMessageBox("Программа уже запущена", "Ошибка", wxOK | wxICON_ERROR);
        exit(1);
    }

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    m_taskInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 30));
    m_addButton = new wxButton(panel, 1001, "Добавить", wxDefaultPosition, wxSize(100, 30));
    m_taskList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(300, 150));

    wxButton* deleteButton = new wxButton(panel, 1002, "Удалить", wxDefaultPosition, wxSize(100, 30));

    sizer->Add(m_taskInput, 0, wxALL | wxCENTER, 10);
    sizer->Add(m_addButton, 0, wxALL | wxCENTER, 10);
    sizer->Add(m_taskList, 0, wxALL | wxCENTER, 10);
    sizer->Add(deleteButton, 0, wxALL | wxCENTER, 10);

    panel->SetSizer(sizer);

    LoadTasks();
    m_addButton->Disable(); // Отключаем кнопку "Добавить", если текстовое поле пустое

    m_taskInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
        m_addButton->Enable(!m_taskInput->GetValue().IsEmpty());
    });

    SetSizeHints(400, 300);
}

void TodoFrame::OnAdd(wxCommandEvent& event) {
    wxString task = m_taskInput->GetValue();
    if (!task.IsEmpty()) {
        tasks.push_back(std::string(task.mb_str()));
        m_taskList->Append(task);
        m_taskInput->Clear();
        m_addButton->Disable();
        SaveTasks();
    }
}

void TodoFrame::OnDelete(wxCommandEvent& event) {
    int selectedIndex = m_taskList->GetSelection();
    if (selectedIndex != wxNOT_FOUND) {
        tasks.erase(tasks.begin() + selectedIndex);
        m_taskList->Delete(selectedIndex);
        SaveTasks();
    }
}

void TodoFrame::OnExit(wxCommandEvent& event) {
    SaveTasks();
    Close(true);
}

void TodoFrame::LoadTasks() {
    std::ifstream file(taskFile);
    if (file.is_open()) {
        std::string task;
        while (getline(file, task)) {
            tasks.push_back(task);
            m_taskList->Append(task);
        }
        file.close();
    }
}

void TodoFrame::SaveTasks() {
    std::ofstream file(taskFile, std::ofstream::trunc);
    if (file.is_open()) {
        for (const auto& task : tasks) {
            file << task << std::endl;
        }
        file.close();
    }
}

bool TodoFrame::IsAppRunning() {
    // Проверка на наличие другого экземпляра приложения
    std::ifstream lockFile("/tmp/todolist.lock");
    if (lockFile.is_open()) {
        return true;
    }
    std::ofstream lockFileOut("/tmp/todolist.lock");
    lockFileOut.close();
    return false;
}

wxIMPLEMENT_APP(TodoApp);

