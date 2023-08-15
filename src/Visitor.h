﻿#ifndef VISITOR_H
#define VISITOR_H

#include <string>

// Forward declare different kind of Node
class Node;
class Group;
class Switch;
class Primitive;
class Scene;
class Surface;
class ImageSurface;
class FrameBufferSurface;
class LineStrip;
class LineSquare;
class LineCircle;
class Mesh;
class Frame;
class Handles;
class Symbol;
class Disk;
class Character;
class Stream;
class MediaPlayer;
class Shader;
class ImageShader;
class MaskShader;
class ImageProcessingShader;

class Source;
class MediaSource;
class StreamSource;
class PatternSource;
class DeviceSource;
class ScreenCaptureSource;
class GenericStreamSource;
class SrtReceiverSource;
class SessionFileSource;
class SessionGroupSource;
class RenderSource;
class CloneSource;
class NetworkSource;
class MixingGroup;
class MultiFileSource;

class FrameBufferFilter;
class PassthroughFilter;
class DelayFilter;
class ResampleFilter;
class BlurFilter;
class SharpenFilter;
class SmoothFilter;
class EdgeFilter;
class AlphaFilter;
class ImageFilter;

class SourceCallback;
class ValueSourceCallback;
class SetAlpha;
class SetDepth;
class SetGeometry;
class SetGamma;
class Loom;
class Grab;
class Resize;
class Turn;
class Play;
class PlayFastForward;


// Declares the interface for the visitors
class Visitor {

public:
    // Need to declare overloads for basic kind of Nodes to visit
    virtual void visit (Scene&) = 0;
    virtual void visit (Node&) = 0;
    virtual void visit (Primitive&) = 0;
    virtual void visit (Group&) = 0;
    virtual void visit (Switch&) = 0;

    // not mandatory for all others
    virtual void visit (Surface&) {}
    virtual void visit (ImageSurface&) {}
    virtual void visit (FrameBufferSurface&) {}
    virtual void visit (LineStrip&)  {}
    virtual void visit (LineSquare&) {}
    virtual void visit (Mesh&) {}
    virtual void visit (Frame&) {}
    virtual void visit (Handles&) {}
    virtual void visit (Symbol&) {}
    virtual void visit (Disk&) {}
    virtual void visit (Character&) {}
    virtual void visit (Shader&) {}
    virtual void visit (ImageShader&) {}
    virtual void visit (MaskShader&) {}
    virtual void visit (ImageProcessingShader&) {}

    // utility
    virtual void visit (Stream&) {}
    virtual void visit (MediaPlayer&) {}
    virtual void visit (MixingGroup&) {}
    virtual void visit (Source&) {}
    virtual void visit (MediaSource&) {}
    virtual void visit (StreamSource&) {}
    virtual void visit (NetworkSource&) {}
    virtual void visit (SrtReceiverSource&) {}
    virtual void visit (GenericStreamSource&) {}
    virtual void visit (DeviceSource&) {}
    virtual void visit (ScreenCaptureSource&) {}
    virtual void visit (PatternSource&) {}
    virtual void visit (SessionFileSource&) {}
    virtual void visit (SessionGroupSource&) {}
    virtual void visit (RenderSource&) {}
    virtual void visit (CloneSource&) {}
    virtual void visit (MultiFileSource&) {}

    virtual void visit (FrameBufferFilter&) {}
    virtual void visit (PassthroughFilter&) {}
    virtual void visit (DelayFilter&) {}
    virtual void visit (ResampleFilter&) {}
    virtual void visit (BlurFilter&) {}
    virtual void visit (SharpenFilter&) {}
    virtual void visit (SmoothFilter&) {}
    virtual void visit (EdgeFilter&) {}
    virtual void visit (AlphaFilter&) {}
    virtual void visit (ImageFilter&) {}

    virtual void visit (SourceCallback&) {}
    virtual void visit (ValueSourceCallback&) {}
    virtual void visit (SetAlpha&) {}
    virtual void visit (SetDepth&) {}
    virtual void visit (SetGeometry&) {}
    virtual void visit (SetGamma&) {}
    virtual void visit (Loom&) {}
    virtual void visit (Grab&) {}
    virtual void visit (Resize&) {}
    virtual void visit (Turn&) {}
    virtual void visit (Play&) {}
    virtual void visit (PlayFastForward&) {}
};


#endif // VISITOR_H
