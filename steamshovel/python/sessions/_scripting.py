from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Any, Callable
    from icecube.icetray import I3Frame
    from icecube._dataclasses import I3Geometry
    from icecube import shovelart

    class GLWidget:
        scenario = shovelart.Scenario()
        def setCameraLoc(self, x: float, y: float, z: float) -> None: ...
        def setCameraPivot(self, x: float, y: float, z: float) -> None: ...
        def setCameraOrientation(self, x: float, y: float, z: float) -> None: ...
        cameraLock: bool
        perspectiveView: bool
        backgroundColor: shovelart.PyQColor
    
    class Timeline:
        rangeFinder: str
        def setEventTimeWindow(self, arg2: float, arg3: float) -> None: ...
        time: float
        minTime: float
        maxTime: float
    
    class FrameFilter:
        code: str

    class FileService:
        def advanceFrame(self, delta: int) -> bool: ...
        def selectFrame(self, index: int) -> bool: ...
        def openLocalFile(self, path: str) -> None: ...
        def openRemoteFile(self, url: str) -> None: ...
        def setFrameFilter(self, filter: str) -> None: ...

        @property
        def openFiles(self) -> "list[int]": ...
        @property
        def currentIndex(self) -> int: ...
        @property
        def currentFrame(self) -> "None|I3Frame": ...
        @property
        def framesAreProcessed(self) -> bool: ...
        @property
        def nFrames(self) -> int: ...
        defaultGeometry: "I3Geometry"

    class MovieEngine:
        def cancelCurrentProduction(self) -> None: ...
        duration: float
        encoder: str
        endtime: float
        fps: int
        guiProgress: bool
        height: int
        imageOutputDir: str
        movieFileName: str
        nframes: int
        objectName: str
        def produceMovie(self, path: str) -> None: ...
        def produceStills(self, dir: str) -> None: ...
        rescaleFlag: bool
        rotPerFrame: float
        rotation: float
        scaleFactor: float
        def setPreframeHook(self, hook: "Callable[[int],Any]") -> None: ...
        starttime: float
        timeStepPerFrame: float
        width: int

    class App:
        def quit(self): ...
        @property
        def files(self) -> FileService: ...

    class Window:
        @property
        def gl(self) -> GLWidget: ...
        @property
        def timeline(self) -> Timeline: ...
        @property
        def frame_filter(self) -> FrameFilter: ...
        @property
        def movieEngine(self) -> MovieEngine: ...
        activeView: bool

    app = App()
    frame = I3Frame()
    window = Window()

    __all__ = ["app", "frame", "window"]