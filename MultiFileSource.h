#ifndef MULTIFILESOURCE_H
#define MULTIFILESOURCE_H

#include <string>
#include <list>

#include "StreamSource.h"

struct MultiFileSequence {
    std::string location;
    std::string codec;
    uint width;
    uint height;
    int min;
    int max;

    MultiFileSequence ();
    MultiFileSequence (const std::list<std::string> &list_files);
    bool valid () const;
    MultiFileSequence& operator = (const MultiFileSequence& b);
    bool operator != (const MultiFileSequence& b);
};

class MultiFile : public Stream
{
public:
    MultiFile ();
    void open (const MultiFileSequence &sequence, uint framerate = 30);
    void close () override;

    // dynamic change of gstreamer multifile source properties
    void setProperties(int begin, int end, int loop);

protected:
    GstElement *src_ ;
};

class MultiFileSource : public StreamSource
{
public:
    MultiFileSource (uint64_t id = 0);

    // Source interface
    void accept (Visitor& v) override;

    // StreamSource interface
    Stream *stream () const override { return stream_; }
    glm::ivec2 icon () const override { return glm::ivec2(3, 9); }

    // specific interface
    void setFiles (const std::list<std::string> &list_files, uint framerate);

    void setSequence (const MultiFileSequence &sequence, uint framerate);
    inline MultiFileSequence sequence () const { return sequence_; }

    void setFramerate (uint fps);
    inline uint framerate () const { return framerate_; }

    void setLoop (bool on);
    inline bool loop () const { return loop_ > 0 ? true : false; }

    void setRange (int begin, int end);
    inline int begin() const { return begin_; }
    inline int end  () const { return end_;   }

    MultiFile *multifile () const;

private:
    MultiFileSequence sequence_;
    uint framerate_;
    int  begin_, end_, loop_;

};


#endif // MULTIFILESOURCE_H
