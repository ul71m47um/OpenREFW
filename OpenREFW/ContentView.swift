internal import UniformTypeIdentifiers
import OpenREFWCore
import Foundation
import SwiftUI

private struct FileTab: Identifiable {
    public let id: UUID = UUID()
    public let name: String
    public let text: AttributedString
}

private struct Executable: Identifiable {
    public let id: UUID = UUID()
    public let sections: [Core.Section]
    public let name: String
}

struct ContentView: View {
    @State private var files: [Executable] = []
    @State private var tabs: [FileTab] = []
    @State private var currentTab: UUID? = nil
    @State private var status: String = "Ready"
    @State private var presented: Bool = false
    @State private var targeted: Bool = false

    private var tabsView: some View {
        TabView(selection: Binding<UUID?>(
            get: { self.currentTab },
            set: { self.currentTab = $0 }
        )) {
            ForEach(self.tabs) { tab in
                TabContentView(tab: tab)
                    .tag(Optional.some(tab.id))
                    .tabItem { TabLabelView(name: tab.name) }
            }
        }
        .tabViewStyle(.grouped)
    }

    public var body: some View {
        VStack {
            ZStack {
                if tabs.isEmpty {
                    VStack(spacing: 12) {
                        Image(systemName: "doc.text.magnifyingglass")
                            .font(.system(size: 48))
                            .foregroundStyle(.secondary)
                        Text("No files are loaded")
                            .font(.largeTitle.bold())
                            .foregroundStyle(.secondary)
                        Text("Open a file with menu \"File -> Open File...\"")
                            .font(.title2)
                            .foregroundStyle(.secondary)
                        Text("Drag and drop files from Finder")
                            .font(.title2)
                            .foregroundStyle(.secondary)
                    }
                } else {
                    NavigationSplitView {
                        TreeContent(files: self.files)
                    } detail: {
                        tabsView
                    }
                }
            }
        }
        .toolbar {
            ToolbarItemGroup(placement: .navigation) {
                Button {
                    presented = true
                } label: {
                    Image(systemName: "folder")
                }
            }
        }
        .fileImporter(isPresented: self.$presented, allowedContentTypes: [.executable]) { result in
            switch result {
            case .success(let url):
                self.push(path: url)
            case .failure:
                break
            }
            
            self.presented = false
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        .background(
            targeted
                ? Color.accentColor.opacity(0.15)
                : Color(nsColor: .textBackgroundColor)
        )
        .overlay {
            RoundedRectangle(cornerRadius: 8)
                .stroke(targeted ? Color.accentColor : .clear, lineWidth: 2)
                .padding(4)
        }
        .onDrop(of: ["public.url"], isTargeted: self.$targeted) { providers in
            guard let provider: NSItemProvider = providers.first else { return false }

            _ = provider.loadObject(ofClass: URL.self) { url, _ in
                guard let url: URL = url else { return }
                self.push(path: url)
            }

            return true
        }
        .safeAreaInset(edge: .bottom, spacing: 0) {
            HStack {
                Text("Status: \(self.status)")
                    .foregroundStyle(.secondary)
            }
            .padding(.horizontal, 10)
            .frame(maxWidth: .infinity)
            .frame(height: 24)
            .background(.bar)
        }
    }
    
    public func push(path: URL) -> Void {
        DispatchQueue.global().async {
            self.status = "Disassembling..."
            let text: AttributedString = Disassembler().Disassemble(path)
            self.status = "Ready"
            
            DispatchQueue.main.async {
                self.files.append(
                    Executable(
                        sections: Disassembler().Parse(path),
                        name: String(path.absoluteString.trimmingPrefix("file://"))
                    )
                )
                let tab: FileTab = FileTab(
                    name: String("\(path.absoluteString.trimmingPrefix("file://")) - disassembled"),
                    text: text
                )
                self.tabs.append(tab)
                self.currentTab = tab.id
                self.presented = false
            }
        }
    }
}

public struct AboutView: View {
    public var body: some View {
        ScrollView {
            VStack {
                Text("OpenREFW - Open Reverse-Engeneering FrameWork")
                    .font(.largeTitle)
                Text("Version 0.1.0")
                
                Spacer()
                
                ScrollView {
                    Text(
                        #"""
                            Copyright 2026 ul71m47um
                        
                            Licensed under the Apache License, Version 2.0 (the "License");
                            you may not use this file except in compliance with the License.
                            You may obtain a copy of the License at
                        
                                http://www.apache.org/licenses/LICENSE-2.0
                        
                            Unless required by applicable law or agreed to in writing, software
                            distributed under the License is distributed on an "AS IS" BASIS,
                            WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
                            See the License for the specific language governing permissions and
                            limitations under the License.
                        """#
                    )
                }
            }
        }
    }
}

private struct DisassembledView: View {
    public let text: AttributedString
    private var lines: [AttributedString]
    
    public var body: some View {
        ScrollView {
            LazyVStack(alignment: .leading, spacing: 0) {
                ForEach(lines.indices, id: \.self) { index in
                    Text(lines[index])
                        .fixedSize(horizontal: false, vertical: true)
                }
            }
        }
        .padding(.horizontal)
        .padding(.vertical, 8)
    }
    
    init(text: AttributedString) {
        self.text = text
        var result: [AttributedString] = []

        let nsAttr: NSAttributedString = NSAttributedString(text)
        let nsString: NSString = nsAttr.string as NSString

        nsString.enumerateSubstrings(
            in: NSRange(location: 0, length: nsString.length),
            options: [.byLines]
        ) { _, range, _, _ in
            result.append(AttributedString(nsAttr.attributedSubstring(from: range)))
        }
        
        self.lines = result
    }
}

private struct TabContentView: View {
    public let tab: FileTab

    public var body: some View {
        DisassembledView(text: tab.text)
            .frame(maxWidth: .infinity, alignment: .leading)
            .textSelection(.enabled)
            .background(Color(nsColor: .textBackgroundColor))
    }
}

private struct TabLabelView: View {
    public let name: String
    public var body: some View {
        Text(name)
            .fixedSize()
    }
}

private struct TreeContent: View {
    @State private var text: String = String()
    private var labels: [String] {
        var labels: [String] = []
        
        for exec in self.files {
            for sect in exec.sections {
                for label in Array(
                    sect.labels.filter {
                        self.text.isEmpty || String($0)
                            .localizedCaseInsensitiveContains(self.text)
                    }
                ) {
                    labels.append(String(label))
                }
            }
        }
        
        return labels
    }
    public var files: [Executable] = []
    
    public var body: some View {
        NavigationStack {
            List {
                if (self.text.isEmpty) {
                    ForEach(self.files) { exec in
                        ExecView(exec: exec)
                    }
                } else {
                    ForEach(self.labels, id: \.self) { label in
                        HStack {
                            if !label.matches(of: #/^__TEXT/#).isEmpty {
                                Text("f")
                                    .italic()
                            }
                            Text(label)
                        }
                    }
                }
            }
            .searchable(
                text: self.$text,
                prompt: "Search symbol..."
            )
        }
    }
}

private struct ExecView: View {
    public let exec: Executable
    
    public var body: some View {
        DisclosureGroup(exec.name) {
            ForEach(exec.sections, id: \.name) { sect in
                SectionView(section: sect)
            }
        }
    }
}

private struct SectionView: View {
    public let section: Core.Section
    public let labels: [String]
    
    public var body: some View {
        DisclosureGroup(String(copying: section.name.utf8Span!)) {
            ForEach(labels, id: \.self) { label in
                HStack {
                    if self.section.name.contains(std.string("__TEXT")) {
                        Text("f")
                            .italic()
                    }
                    Text(label)
                }
            }
        }
    }
    
    init(section: Core.Section) {
        self.section = section
        self.labels = section.labels.map { String(copying: $0.utf8Span!) }
    }
}
