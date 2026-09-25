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

    private func highlight(_ line: String) -> AttributedString {
        var str: AttributedString = AttributedString(line)

        let reg: Regex<Substring> = #/(?:\sw|x|b|h|s|d|q|v)\d+/#
        let imm: Regex<Substring> = #/#-*\d\.*x*[0-9a-f]*/#
        let addr: Regex<Substring> = #/(?:\s)0x[a-f0-9]+/#
        let label: Regex<Substring> = #/(?:\s|^)\_[\S\s]+/#
        let comment: Regex<Substring> = #/;[\s\S]+$/#
        let begin: Regex<Substring> = #/^0x[0-9a-f]+:/#

        func apply(
            regex: Regex<Substring>,
            color: Color
        ) {
            var container: AttributeContainer = AttributeContainer()
            container.foregroundColor = color

            for match in line.matches(of: regex) {
                let range: Range = match.range

                if let lower: AttributedString.Index = AttributedString.Index(
                    range.lowerBound,
                    within: str
                ),
                let upper: AttributedString.Index = AttributedString.Index(
                    range.upperBound,
                    within: str
                ) {
                    str[lower..<upper].mergeAttributes(container)
                }
            }
        }

        apply(regex: reg, color: .blue)
        apply(regex: imm, color: .red)
        apply(regex: addr, color: .yellow)
        apply(regex: label, color: .yellow)
        apply(regex: comment, color: .gray)
        apply(regex: begin, color: .gray)

        apply(
            regex: #/\]!*/#,
            color: .white
        )

        return str
    }

    public func disassemble(_ path: URL) async -> AttributedString {
        let filePath = String(
            path.path
        )

        return await withCheckedContinuation { continuation in
            self.queue.async {
                var text: AttributedString = AttributedString()

                for line in self.disxx.Disassemble(
                    std.string(filePath)
                ) {

                    let line: String = String(
                        copying: line.utf8Span!
                    )

                    text += self.highlight(line + "\n")
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
                let sections: [Core.Section] = Array(
                    self.disxx.ParseSections(
                        std.string(filePath)
                    )
                )

                continuation.resume(
                    returning: sections
                )
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
    private var view: ContentView = ContentView()
    
    public var body: some Scene {
        WindowGroup {
            self.view
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
                        self.view.push(path: url)
                        
                    case .failure:
                        break
                    }
                    
                    self.presented = false
                }
                
                Button("Close") {
                    self.view.closeCurrent()
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
