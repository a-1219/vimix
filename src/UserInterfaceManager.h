#ifndef __UI_MANAGER_H_
#define __UI_MANAGER_H_

#include "Session.h"
#include <string>
#include <array>
#include <list>
#include <future>
#include <variant>

#include <gst/gstutils.h>

#define NAV_COUNT 68
#define NAV_MAX   64
#define NAV_NEW   65
#define NAV_MENU  66
#define NAV_TRANS 67

#ifdef APPLE
#define CTRL_MOD "Cmd+"
#define ALT_MOD "Option+"
#else
#define CTRL_MOD "Ctrl+"
#define ALT_MOD "Alt+"
#endif

#define IMGUI_TITLE_MEDIAPLAYER ICON_FA_PLAY_CIRCLE "  Player"
#define IMGUI_TITLE_TIMER ICON_FA_CLOCK " Timer"
#define IMGUI_TITLE_INPUT_MAPPING ICON_FA_HAND_PAPER " Input Mapping"
#define IMGUI_TITLE_HELP ICON_FA_LIFE_RING "  Help"
#define IMGUI_TITLE_TOOLBOX ICON_FA_HAMSA "  Guru Toolbox"
#define IMGUI_TITLE_SHADEREDITOR ICON_FA_CODE "  Shader Editor"
#define IMGUI_TITLE_PREVIEW ICON_FA_LAPTOP "  Display"

#define MENU_NEW_FILE         ICON_FA_FILE "  New"
#define SHORTCUT_NEW_FILE     CTRL_MOD "W"
#define MENU_OPEN_FILE        ICON_FA_FILE_UPLOAD "  Open"
#define SHORTCUT_OPEN_FILE    CTRL_MOD "O"
#define MENU_REOPEN_FILE      ICON_FA_FILE_UPLOAD "  Re-open"
#define SHORTCUT_REOPEN_FILE  CTRL_MOD "Shift+O"
#define MENU_SAVE_FILE        ICON_FA_FILE_DOWNLOAD "  Save"
#define SHORTCUT_SAVE_FILE    CTRL_MOD "S"
#define MENU_SAVEAS_FILE      ICON_FA_FILE_DOWNLOAD "  Save as"
#define MENU_SAVE_ON_EXIT     ICON_FA_LEVEL_DOWN_ALT "  Save on exit"
#define MENU_OPEN_ON_START    ICON_FA_LEVEL_UP_ALT "  Restore on start"
#define SHORTCUT_SAVEAS_FILE  CTRL_MOD "Shift+S"
#define MENU_QUIT             ICON_FA_POWER_OFF "  Quit"
#define SHORTCUT_QUIT         CTRL_MOD "Q"
#define MENU_CUT              ICON_FA_CUT "  Cut"
#define SHORTCUT_CUT          CTRL_MOD "X"
#define MENU_COPY             ICON_FA_COPY "  Copy"
#define SHORTCUT_COPY         CTRL_MOD "C"
#define MENU_DELETE           ICON_FA_ERASER " Delete"
#define SHORTCUT_DELETE       "Del"
#define ACTION_DELETE         ICON_FA_BACKSPACE " Delete"
#define MENU_PASTE            ICON_FA_PASTE "  Paste"
#define SHORTCUT_PASTE        CTRL_MOD "V"
#define MENU_SELECTALL        ICON_FA_TH_LIST "  Select all"
#define SHORTCUT_SELECTALL    CTRL_MOD "A"
#define MENU_UNDO             ICON_FA_UNDO "  Undo"
#define SHORTCUT_UNDO         CTRL_MOD "Z"
#define MENU_REDO             ICON_FA_REDO "  Redo"
#define SHORTCUT_REDO         CTRL_MOD "Shift+Z"
#define MENU_RECORD           ICON_FA_CIRCLE "  Record"
#define SHORTCUT_RECORD       CTRL_MOD "R"
#define MENU_RECORDCONT       ICON_FA_STOP_CIRCLE "  Save & continue"
#define SHORTCUT_RECORDCONT   CTRL_MOD "Alt+R"
#define MENU_CAPTUREFRAME     ICON_FA_CAMERA_RETRO "  Capture frame"
#define SHORTCUT_CAPTURE_DISPLAY "F11"
#define SHORTCUT_CAPTURE_PLAYER "F10"
#define MENU_CAPTUREGUI       ICON_FA_CAMERA "  Screenshot vimix"
#define SHORTCUT_CAPTURE_GUI  "F9"
#define MENU_OUTPUTDISABLE    ICON_FA_EYE_SLASH " Disable"
#define SHORTCUT_OUTPUTDISABLE "F12"
#define MENU_LARGEPREVIEW     ICON_FA_EXPAND_ARROWS_ALT "   Large preview"
#define SHORTCUT_LARGEPREVIEW "F6"
#define MENU_CLOSE            ICON_FA_TIMES "   Close"
#define DIALOG_FAILED_SOURCE  ICON_FA_EXCLAMATION_TRIANGLE " Source failure"

#define MENU_NOTE             ICON_FA_STICKY_NOTE "   Add sticky note"
#define MENU_METRICS          ICON_FA_TACHOMETER_ALT "  Metrics"
#define MENU_SOURCE_TOOL      ICON_FA_LIST_ALT "  Source toolbar"
#define MENU_HELP             ICON_FA_LIFE_RING "  Help"
#define SHORTCUT_HELP         CTRL_MOD "H"
#define MENU_LOGS             ICON_FA_LIST_UL "  Logs"
#define SHORTCUT_LOGS         CTRL_MOD "L"
#define TOOLTIP_PLAYER        "Player "
#define SHORTCUT_PLAYER       CTRL_MOD "P"
#define TOOLTIP_OUTPUT        "Display "
#define SHORTCUT_OUTPUT       CTRL_MOD "D"
#define TOOLTIP_TIMER         "Timer "
#define SHORTCUT_TIMER        CTRL_MOD "T"
#define TOOLTIP_INPUTS        "Inputs mapping "
#define SHORTCUT_INPUTS       CTRL_MOD "I"
#define TOOLTIP_SHADEREDITOR  "Shader Editor "
#define SHORTCUT_SHADEREDITOR CTRL_MOD "E"
#define TOOLTIP_FULLSCREEN    "Fullscreen "
#define SHORTCUT_FULLSCREEN   CTRL_MOD "F"
#define TOOLTIP_MAIN          "Main menu "
#define SHORTCUT_MAIN         "HOME"
#define TOOLTIP_NEW_SOURCE    "New source "
#define SHORTCUT_NEW_SOURCE   "INS"
#define TOOLTIP_HIDE          "Hide windows "
#define TOOLTIP_SHOW          "Show windows "
#define SHORTCUT_HIDE         "ESC"
#define TOOLTIP_PANEL_VISIBLE "Panel always visible "
#define TOOLTIP_PANEL_AUTO    "Panel auto hide "
#define SHORTCUT_PANEL_MODE   "HOME"

#define LABEL_AUTO_MEDIA_PLAYER ICON_FA_USER_CIRCLE "  User selection"
#define LABEL_STORE_SELECTION "  Create batch"
#define LABEL_EDIT_FADING ICON_FA_RANDOM "  Fade in & out"
#define LABEL_VIDEO_SEQUENCE "  Encode an image sequence"

#include "SourceList.h"
#include "InfoVisitor.h"
#include "DialogToolkit.h"
#include "SessionParser.h"
#include "ImageFilter.h"
#include "Screenshot.h"


struct ImVec2;
class MediaPlayer;
class FrameBufferImage;
class FrameGrabber;
class VideoRecorder;
class VideoBroadcast;
class ShmdataBroadcast;
class Loopback;

class SourcePreview {

    Source *source_;
    std::string label_;
    bool reset_;

public:
    SourcePreview();

    void setSource(Source *s = nullptr, const std::string &label = "");
    Source *getSource();

    void Render(float width);
    bool ready() const;
    inline bool filled() const { return source_ != nullptr; }
};

class Thumbnail
{
    float aspect_ratio_;
    uint texture_;

public:
    Thumbnail();
    ~Thumbnail();

    void reset();
    void fill (const FrameBufferImage *image);
    bool filled();
    void Render(float width);
};

class Navigator
{
    // geometry left bar & pannel
    float width_;
    float height_;
    float pannel_width_;
    float padding_width_;

    // behavior pannel
    bool show_config_;
    bool pannel_visible_;
    float pannel_alpha_;
    bool view_pannel_visible;
    bool selected_button[NAV_COUNT];
    int  pattern_type;
    bool custom_pipeline;
    bool custom_connected;
    void clearButtonSelection();
    void clearNewPannel();
    void applyButtonSelection(int index);

    // side pannels
    void RenderSourcePannel(Source *s);
    void RenderMainPannel();
    void RenderMainPannelVimix();
    void RenderMainPannelSettings();
    void RenderTransitionPannel();
    void RenderNewPannel();
    void RenderViewPannel(ImVec2 draw_pos, ImVec2 draw_size);

public:

    typedef enum {
        SOURCE_FILE = 0,
        SOURCE_SEQUENCE,
        SOURCE_CONNECTED,
        SOURCE_GENERATED,
        SOURCE_INTERNAL,
        SOURCE_TYPES
    } NewSourceType;

    Navigator();
    void Render();

    bool pannelVisible();
    void hidePannel();
    void showPannelSource(int index);
    void togglePannelMenu();
    void togglePannelNew();
    void showConfig();
    void togglePannelAutoHide();

    typedef enum {
        MEDIA_RECENT = 0,
        MEDIA_RECORDING,
        MEDIA_FOLDER
    } MediaCreateMode;
    void setNewMedia(MediaCreateMode mode, std::string path = std::string());


private:
    // for new source panel
    SourcePreview new_source_preview_;
    std::list<std::string> sourceSequenceFiles;
    std::list<std::string> sourceMediaFiles;
    std::string sourceMediaFileCurrent;
    MediaCreateMode new_media_mode;
    bool new_media_mode_changed;
    Source *source_to_replace;
};

class ToolBox
{
    bool show_demo_window;
    bool show_icons_window;
    bool show_sandbox;

public:
    ToolBox();

    void Render();
};

class HelperToolbox
{
    SessionParser parser_;

public:
    HelperToolbox();

    void Render();

};

class WorkspaceWindow
{
    static std::list<WorkspaceWindow *> windows_;

public:
    WorkspaceWindow(const char* name);

    // global access to Workspace control
    static bool clear() { return clear_workspace_enabled; }
    static void toggleClearRestoreWorkspace();
    static void clearWorkspace();
    static void restoreWorkspace(bool instantaneous = false);
    static void notifyWorkspaceSizeChanged(int prev_width, int prev_height, int curr_width, int curr_height);

    // for inherited classes
    virtual void Update();
    virtual bool Visible() const { return true; }

protected:

    static bool clear_workspace_enabled;

    const char *name_;
    struct ImGuiProperties *impl_;
};

class SourceController : public WorkspaceWindow
{
    float min_width_;
    float h_space_;
    float v_space_;
    float scrollbar_;
    float timeline_height_;
    float mediaplayer_height_;
    float buttons_width_;
    float buttons_height_;

    bool play_toggle_request_, replay_request_, capture_request_;
    bool pending_;
    std::string active_label_;
    int active_selection_;
    InfoVisitor info_;
    SourceList selection_;

    // re-usable ui parts
    void DrawButtonBar(ImVec2 bottom, float width);
    int SourceButton(Source *s, ImVec2 framesize);

    // Render the sources dynamically selected
    void RenderSelectedSources();

    // Render a stored selection
    bool selection_context_menu_;
    MediaPlayer *selection_mediaplayer_;
    double selection_target_slower_;
    double selection_target_faster_;
    void RenderSelectionContextMenu();
    void RenderSelection(size_t i);

    // Render a single source
    void RenderSingleSource(Source *s);

    // Render a single media player
    MediaPlayer *mediaplayer_active_;
    bool mediaplayer_edit_fading_;
    bool mediaplayer_edit_pipeline_;
    bool mediaplayer_mode_;
    bool mediaplayer_slider_pressed_;
    float mediaplayer_timeline_zoom_;
    void RenderMediaPlayer(MediaSource *ms);

    // magnifying glass
    bool magnifying_glass;

    // dialog to select frame capture location
    DialogToolkit::OpenFolderDialog *captureFolderDialog;
    Screenshot capture;

public:
    SourceController();

    inline void Play()   { play_toggle_request_  = true; }
    inline void Replay() { replay_request_= true; }
    inline void Capture(){ capture_request_= true; }
    void resetActiveSelection();

    void setVisible(bool on);
    void Render();

    // from WorkspaceWindow
    void Update() override;
    bool Visible() const override;
};

class OutputPreview : public WorkspaceWindow
{
    // frame grabbers
    VideoRecorder *video_recorder_;
    VideoBroadcast *video_broadcaster_;
    ShmdataBroadcast *shm_broadcaster_;
    Loopback *loopback_broadcaster_;

    // delayed trigger for recording
    std::vector< std::future<VideoRecorder *> > _video_recorders;

    // dialog to select record location
    DialogToolkit::OpenFolderDialog *recordFolderDialog;

    // magnifying glass
    bool magnifying_glass;

public:
    OutputPreview();

    void ToggleRecord(bool save_and_continue = false);
    inline bool isRecording() const { return video_recorder_ != nullptr; }

    void ToggleVideoBroadcast();
    inline bool videoBroadcastEnabled() const { return video_broadcaster_ != nullptr; }

    void ToggleSharedMemory();
    inline bool sharedMemoryEnabled() const { return shm_broadcaster_ != nullptr; }

    bool ToggleLoopbackCamera();
    inline bool loopbackCameraEnabled() const { return loopback_broadcaster_!= nullptr; }

    void Render();
    void setVisible(bool on);

    // from WorkspaceWindow
    void Update() override;
    bool Visible() const override;
};

class TimerMetronome : public WorkspaceWindow
{
    std::array< std::string, 2 > timer_menu;
    // clock times
    guint64 start_time_;
    guint64 start_time_hand_;
    guint64 duration_hand_;

public:
    TimerMetronome();

    void Render();
    void setVisible(bool on);

    // from WorkspaceWindow
    bool Visible() const override;
};

class InputMappingInterface : public WorkspaceWindow
{
    std::array< std::string, 4 > input_mode;
    std::array< uint, 4 > current_input_for_mode;
    uint current_input_;

    Target ComboSelectTarget(const Target &current);
    uint ComboSelectCallback(uint current, bool imageprocessing);
    void SliderParametersCallback(SourceCallback *callback, const Target &target);

public:
    InputMappingInterface();

    void Render();
    void setVisible(bool on);

    // from WorkspaceWindow
    bool Visible() const override;
};

class ShaderEditor : public WorkspaceWindow
{
    ImageFilter *current_;
    std::map<ImageFilter *, FilteringProgram> filters_;
    std::promise<std::string> *compilation_;
    std::future<std::string>  compilation_return_;

    bool show_shader_inputs_;
    std::string status_;

public:
    ShaderEditor();

    void Render();
    void setVisible(bool on);

    void BuildShader();

    // from WorkspaceWindow
    bool Visible() const override;
};


class UserInterface
{
    friend class ImGuiVisitor;
    friend class Navigator;
    friend class OutputPreview;

    // Private Constructor
    UserInterface();
    UserInterface(UserInterface const& copy) = delete;
    UserInterface& operator=(UserInterface const& copy) = delete;

public:

    static UserInterface& manager()
    {
        // The only instance
        static UserInterface _instance;
        return _instance;
    }

    // pre-loop initialization
    bool Init();
    // loop update start new frame
    void NewFrame();
    // loop update rendering
    void Render();
    // try to close, return false if cannot
    bool TryClose();
    // Post-loop termination
    void Terminate();
    // Runtime
    uint64_t Runtime() const;

    // status querries
    inline bool keyboardAvailable() const { return keyboard_available; }
    inline bool ctrlModifier() const { return ctrl_modifier_active; }
    inline bool altModifier() const  { return alt_modifier_active; }
    inline bool shiftModifier() const  { return shift_modifier_active; }

    void showPannel(int id = 0);
    void showSourcePanel(Source *s);
    void showSourceEditor(Source *s);

    void StartScreenshot();

protected:

    // internal
    char inifilepath[2048];
    uint64_t start_time;
    bool ctrl_modifier_active;
    bool alt_modifier_active;
    bool shift_modifier_active;
    bool keyboard_available;
    bool show_vimix_about;
    bool show_imgui_about;
    bool show_gst_about;
    bool show_opengl_about;
    int  show_view_navigator;
    int  target_view_navigator;
    unsigned int screenshot_step;
    bool pending_save_on_exit;
    bool show_output_fullview;

    // Dialogs
    DialogToolkit::OpenSessionDialog *sessionopendialog;
    DialogToolkit::OpenSessionDialog *sessionimportdialog;
    DialogToolkit::SaveSessionDialog *sessionsavedialog;

    // objects and windows
    Navigator navigator;
    ToolBox toolbox;
    SourceController sourcecontrol;
    OutputPreview outputcontrol;
    TimerMetronome timercontrol;
    InputMappingInterface inputscontrol;
    ShaderEditor shadercontrol;
    HelperToolbox helpwindow;

    void showMenuFile();
    void showMenuEdit();
    bool saveOrSaveAs(bool force_versioning = false);
    void selectSaveFilename();
    void selectOpenFilename();

    void RenderMetrics (bool* p_open, int* p_corner, int *p_mode);
    void RenderSourceToolbar(bool *p_open, int* p_border, int *p_mode);
    int  RenderViewNavigator(int* shift);
    void RenderOutputView();
    void RenderAbout(bool* p_open);
    void RenderNotes();

    void handleKeyboard();
    void handleMouse();
    void handleScreenshot();
};

#endif /* #define __UI_MANAGER_H_ */

