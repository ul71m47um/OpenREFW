import OpenREFWCore
import Foundation
import SwiftUI

public class Disassembler {
    private func highlight(_ line: String) -> AttributedString {
        let parts: [Substring] = line.split(whereSeparator: \.isWhitespace)

        guard parts.count >= 2 else {
            return AttributedString(line)
        }

        var result: AttributedString = AttributedString()

        let address: AttributedString = AttributedString(parts[0] + " ")
        result += address

        var instruction: AttributedString = AttributedString(parts[1] + " ")
        instruction.foregroundColor = .purple
        result += instruction

        for operand in parts.dropFirst(2) {
            var operandText: AttributedString = AttributedString(operand + " ")
            operandText.foregroundColor = .blue
            result += operandText
        }

        return result + "\n"
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

@main
struct OpenREFWApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
