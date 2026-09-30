internal import UniformTypeIdentifiers
import OpenREFWCore
import Foundation
import CxxStdlib
import SwiftUI

public final class Backend {
    public enum Event: Sendable {
        case message(String)
    }

    public let events: AsyncStream<Event>

    private let continuation: AsyncStream<Event>.Continuation

    private let queue = DispatchQueue(
        label: "io.github.ul71m47um.OpenREFW.backend",
        qos: .userInitiated
    )

    private lazy var disxx: Core = {
        Core(
            Unmanaged
                .passUnretained(self)
                .toOpaque(),

            Core.Callback { ctx, message in
                guard let ctx else {
                    return
                }

                let backend: Backend = Unmanaged<Backend>
                    .fromOpaque(ctx)
                    .takeUnretainedValue()

                backend.continuation.yield(
                    .message(String(message))
                )
            }
        )
    }()

    public init() {
        let stream = AsyncStream<Event>.makeStream(
            of: Event.self,
            bufferingPolicy: .unbounded
        )

        self.events = stream.stream
        self.continuation = stream.continuation
    }

    public func disassemble(_ path: URL) async -> String {
        let filePath = String(
            path.path
        )

        return await withCheckedContinuation { continuation in
            self.queue.async {
                var text: String = String()

                for line in self.disxx.Disassemble(std.string(filePath)) {
                    let line: String = String(
                        copying: line.utf8Span!
                    )

                    text += "\(line)\n"
                }

                continuation.resume(
                    returning: text
                )
            }
        }
    }

    public func parse(_ path: URL) async -> [Core.Section] {
        let filePath: String = String(path.path)

        return await withCheckedContinuation { continuation in
            queue.async {
                let sections: [Core.Section] = Array(self.disxx.ParseSections(std.string(filePath)))
                
                continuation.resume(returning: sections)
            }
        }
    }
}

struct OpenREFWAppCommands: Commands {
    @Environment(\.openWindow) private var mkwin
    
    var body: some Commands {
        CommandGroup(replacing: .appInfo) {
            Button("About OpenREFW") {
                mkwin(id: "about")
            }
        }
    }
}

@main
struct OpenREFWApp: App {
    @State private var presented: Bool = false
    @StateObject private var state: ViewState = ViewState()
    
    public var body: some Scene {
        WindowGroup {
            ContentView(state: self.state)
        }
        .commandsRemoved()
        .commands {
            CommandMenu("File") {
                Button("Open file...") {
                    self.presented = true
                }
                .fileImporter(
                    isPresented: self.$presented,
                    allowedContentTypes: [.executable]
                ) { result in
                    switch result {
                    case .success(let url):
                        self.state.push(path: url)
                        
                    case .failure:
                        break
                    }
                    
                    self.presented = false
                }
                
                Button("Close") {
                    self.state.closeCurrent()
                }
            }
        }
        
        WindowGroup("About OpenREFW", id: "about") {
            AboutView()
        }
        .windowResizability(.contentSize)
        .commandsRemoved()
        .commands {
            OpenREFWAppCommands()
        }
    }
}
