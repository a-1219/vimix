#ifndef SESSIONCREATOR_H
#define SESSIONCREATOR_H

#include <list>
#include <map>
#include <tinyxml2.h>

#include "Visitor.h"
#include "SourceList.h"

class Session;
class FrameBufferImage;


class SessionLoader : public Visitor {

public:

    SessionLoader(Session *session = nullptr, uint level = 0);
    inline Session *session() const { return session_; }

    void load(tinyxml2::XMLElement *sessionNode);
    std::map< uint64_t, Source* > getSources() const;
    std::list< SourceList > getMixingGroups() const;

    typedef enum {
        CLONE,
        REPLACE,
        DUPLICATE
    } Mode;
    Source *createSource(tinyxml2::XMLElement *sourceNode, Mode mode = CLONE);
    Source *recreateSource(Source *s);

    static bool isClipboard(const std::string &clipboard);
    static tinyxml2::XMLElement* firstSourceElement(const std::string &clipboard, tinyxml2::XMLDocument &xmlDoc);
    static void applyImageProcessing(const Source &s, const std::string &clipboard);
    //TODO static void applyMask(const Source &s, const std::string &clipboard);

    // Elements of Scene
    void visit (Node& n) override;
    void visit (Scene&) override {}
    void visit (Group&) override {}
    void visit (Switch&) override {}
    void visit (Primitive&) override {}

    // Elements with attributes
    void visit (MediaPlayer& n) override;
    void visit (Stream& n) override;
    void visit (Shader& n) override;
    void visit (ImageShader& n) override;
    void visit (MaskShader& n) override;
    void visit (ImageProcessingShader& n) override;

    // Sources
    void visit (Source& s) override;
    void visit (MediaSource& s) override;
    void visit (StreamSource& s) override;
    void visit (SessionFileSource& s) override;
    void visit (SessionGroupSource& s) override;
    void visit (RenderSource& s) override;
    void visit (PatternSource& s) override;
    void visit (DeviceSource& s) override;
    void visit (ScreenCaptureSource& s) override;
    void visit (NetworkSource& s) override;
    void visit (MultiFileSource& s) override;
    void visit (GenericStreamSource& s) override;
    void visit (SrtReceiverSource& s) override;

    void visit (CloneSource& s) override;
    void visit (FrameBufferFilter&) override;
    void visit (DelayFilter&) override;
    void visit (ResampleFilter&) override;
    void visit (BlurFilter&) override;
    void visit (SharpenFilter&) override;
    void visit (EdgeFilter&) override;
    void visit (SmoothFilter&) override;
    void visit (AlphaFilter&) override;
    void visit (ImageFilter&) override;

    // callbacks
    void visit (SourceCallback&) override;
    void visit (ValueSourceCallback&) override;
    void visit (SetAlpha&) override;
    void visit (SetDepth&) override;
    void visit (SetGeometry&) override;
    void visit (SetGamma&) override;
    void visit (Loom&) override;
    void visit (Grab&) override;
    void visit (Resize&) override;
    void visit (Turn&) override;
    void visit (Play&) override;
    void visit (PlayFastForward&) override;

    static void XMLToNode(const tinyxml2::XMLElement *xml, Node &n);
    static void XMLToSourcecore(tinyxml2::XMLElement *xml, SourceCore &s);
    static FrameBufferImage *XMLToImage(const tinyxml2::XMLElement *xml);

protected:
    // result created session
    Session *session_;
    std::string sessionFilePath_;
    // parsing current xml
    tinyxml2::XMLElement *xmlCurrent_;
    // level of loading imbricated sessions and recursion
    uint level_;
    // map of correspondance from xml source id (key) to new source pointer (value)
    std::map< uint64_t, Source* > sources_id_;
    // list of groups (lists of xml source id)
    std::list< SourceIdList > groups_sources_id_;

};

struct SessionInformation {
    std::string description;
    FrameBufferImage *thumbnail;
    bool user_thumbnail_;
    SessionInformation() {
        description = "";
        thumbnail = nullptr;
        user_thumbnail_ = false;
    }
};

class SessionCreator : public SessionLoader {

    tinyxml2::XMLDocument xmlDoc_;

    void loadConfig(tinyxml2::XMLElement *viewsNode);
    void loadNotes(tinyxml2::XMLElement *notesNode);
    void loadPlayGroups(tinyxml2::XMLElement *playlistsNode);
    void loadSnapshots(tinyxml2::XMLElement *snapshotNode);
    void loadInputCallbacks(tinyxml2::XMLElement *inputsNode);

public:
    SessionCreator(uint level = 0);

    void load(const std::string& filename);

    static SessionInformation info(const std::string& filename);
};

#endif // SESSIONCREATOR_H
