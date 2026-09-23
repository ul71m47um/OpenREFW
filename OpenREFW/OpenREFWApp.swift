internal import UniformTypeIdentifiers
import OpenREFWCore
import Foundation
import SwiftUI

public class Disassembler {
    private func highlight(_ line: String) -> AttributedString {
        var str: AttributedString = AttributedString(line)

        let reg: Regex<Substring>  = #/(?:\sw|x|b|h|s|d|q|v)\d+/#
        let imm: Regex<Substring>  = #/#-*\d\.*x*[0-9a-f]*/#
        let addr: Regex<Substring> = #/(?:\s)0x[a-f0-9]+/#
        let label: Regex<Substring> = #/(?:\s|^)_[\S\s]+/#
        let comment: Regex<Substring> = #/;[\s\S]+$/#
        let begin: Regex<Substring> = #/^0x[0-9a-f]+:/#
        
        func apply(regex: Regex<Substring>, color: Color) {
            var container: AttributeContainer = AttributeContainer()
            container.foregroundColor = color

            for match in line.matches(of: regex) {
                let r = match.range
                if let lower = AttributedString.Index(r.lowerBound, within: str),
                   let upper = AttributedString.Index(r.upperBound, within: str) {
                    str[lower..<upper].mergeAttributes(container)
                }
            }
        }

        apply(regex: reg,  color: .blue)
        apply(regex: imm,  color: .red)
        apply(regex: addr, color: .yellow)
        apply(regex: label, color: .yellow)
        apply(regex: comment, color: .gray)
        apply(regex: begin, color: .gray)
        
        // Apply normal color back to "]!" substrings
        apply(regex: #/\]\!*/#, color: .white)

        return str
    }
    
    public func Disassemble(_ path: URL) -> AttributedString {
        var text: AttributedString = AttributedString()
        for line in Core.Disassemble(std.string(String(path.absoluteString.trimmingPrefix("file://")))) {
            text += self.highlight(String(copying: line.utf8Span!) + "\n")
        }
        
        return text
    }
    
    public func Parse(_ path: URL) -> [Core.Section] {
        return Array(
            Core.ParseSections(
                std.string(
                    String(
                        path
                            .absoluteString
                            .trimmingPrefix("file://")
                    )
                )
            )
        )
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
                }.fileImporter(
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
