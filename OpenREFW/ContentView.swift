import SwiftUI
import OpenREFWCore

func mktxt() -> String {
    var str: String = String()
    for line in OpenREFWCore.Core.Disassemble("") {
        str += "\(String(copying: line.utf8Span!))\n"
    }
    
    return str
}

struct ContentView: View {
    @State var text: String = mktxt()
    
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundStyle(.tint)
            TextEditor(text: $text)
                .padding()
        }
    }
}

#Preview {
    ContentView()
}
