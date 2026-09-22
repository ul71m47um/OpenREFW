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
    @State private var presented: Bool = false
    @State private var targeted: Bool = false

    private var tabsView: some View {
        TabView(selection: Binding<UUID?>(
            get: { self.currentTab },
            set: { self.currentTab = $0 }
        )) {
            ForEach(self.tabs) { tab in
                TabContent(tab: tab)
                    .tag(Optional.some(tab.id))
                    .tabItem { TabLabel(name: tab.name) }
            }
        }
        .tabViewStyle(.grouped)
    }

    var body: some View {
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
                let text: AttributedString = Disassembler().Disassemble(url)
                self.files.append(
                    Executable(
                        sections: Disassembler().Parse(url),
                        name: String(url.absoluteString.trimmingPrefix("file://"))
                    )
                )
                let tab: FileTab = FileTab(
                    name: String(url.absoluteString.trimmingPrefix("file://")),
                    text: text
                )
                self.tabs.append(tab)
                self.currentTab = tab.id
                self.presented = false
                
            case .failure:
                break
            }
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
                DispatchQueue.main.async {
                    let text: AttributedString = Disassembler().Disassemble(url)
                    self.files.append(
                        Executable(
                            sections: Disassembler().Parse(url),
                            name: String(url.absoluteString.trimmingPrefix("file://"))
                        )
                    )
                    let newTab = FileTab(name: url.lastPathComponent, text: text)
                    self.tabs.append(newTab)
                    self.currentTab = newTab.id
                }
            }

            return true
        }
    }
}

private struct DisassembledView: View {
    public let text: AttributedString
    private var lines: [AttributedString]
    
    var body: some View {
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

private struct TabContent: View {
    public let tab: FileTab

    var body: some View {
        DisassembledView(text: tab.text)
            .frame(maxWidth: .infinity, alignment: .leading)
            .textSelection(.enabled)
            .background(Color(nsColor: .textBackgroundColor))
    }
}

private struct TabLabel: View {
    let name: String
    var body: some View {
        Text(name)
            .fixedSize()
    }
}

private struct TreeContent: View {
    public var files: [Executable] = []
   
    var body: some View {
        List {
            ForEach(files) { exec in
                ExecView(exec: exec)
            }
        }
    }
}

private struct ExecView: View {
    let exec: Executable
    var body: some View {
        DisclosureGroup(exec.name) {
            ForEach(exec.sections, id: \.name) { sect in
                SectionView(section: sect)
            }
        }
    }
}

private struct SectionView: View {
    let section: Core.Section
    let labels: [String]
    
    var body: some View {
        DisclosureGroup(String(copying: section.name.utf8Span!)) {
            ForEach(labels, id: \.self) { label in
                Text(label)
            }
        }
    }
    
    init(section: Core.Section) {
        self.section = section
        self.labels = section.labels.map { String(copying: $0.utf8Span!) }
    }
}
