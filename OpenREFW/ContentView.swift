internal import UniformTypeIdentifiers
import OpenREFWCore
import Foundation
import Combine
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

private final class ViewState: ObservableObject {
    @Published var consoleText: AttributedString = AttributedString()
    @Published var backend: Backend = Backend()
    @Published var files: [Executable] = []
    @Published var tabs: [FileTab] = []
    @Published var currentTab: UUID? = nil
    @Published var status: String = "Ready"
   
    private var manager: UndoManager? = nil
    
    public func setManager(_ manager: UndoManager?) -> Void {
        self.manager = manager
    }
    
    public func push(path: URL) -> Void {
        self.status = "Disassembling..."
        Task { @MainActor in
            let sections: [Core.Section] = await self.backend.parse(path)
            let text: AttributedString = await self.backend.disassemble(path)
            
            let name: String = String(path.absoluteString.trimmingPrefix("file://"))
            let exec: Executable = Executable(
                sections: sections,
                name: name
            )
            let tab: FileTab = FileTab(
                name: "\(name) - disassembled",
                text: text
            )
                
            self.manager?.beginUndoGrouping()
            self.manager?.registerUndo(withTarget: self) { target in
                let rmtab: FileTab? = target.tabs.first { $0.id == tab.id }
                let rmexec: Executable? = target.files.first { $0.id == exec.id }
                target.tabs.removeAll { $0.id == tab.id }
                target.files.removeAll { $0.id == exec.id }
                target.currentTab = target.tabs.last?.id
                
                self.manager?.registerUndo(withTarget: target) { target in
                    if let t: FileTab = rmtab { target.tabs.append(t) }
                    if let e: Executable = rmexec { target.files.append(e) }
                    target.currentTab = tab.id
                }
            }
            self.manager?.setActionName("Open File...")
            self.manager?.endUndoGrouping()
                
            self.files.append(exec)
            self.tabs.append(tab)
            self.currentTab = tab.id
            
            self.status = "Ready"
        }
    }
    
    func close(_ id: UUID) -> Void {
        self.manager?.beginUndoGrouping()
        self.manager?.registerUndo(withTarget: self) { target in
            let restoreTab: FileTab? = target.tabs.first { $0.id == id }
            let restoreExec: Executable? = target.files.first { $0.id == id }
           
            if restoreTab != nil {
                target.tabs.append(restoreTab!)
            }
            if restoreExec != nil {
                target.files.append(restoreExec!)
            }
                
            target.currentTab = target.tabs.last?.id

            self.manager?.registerUndo(withTarget: target) { target in
                target.tabs.removeAll {
                    $0.id == id
                }
                target.files.removeAll {
                    $0.id == id
                }
                
                target.currentTab = nil
            }
        }
        self.manager?.setActionName("Close")
        self.manager?.endUndoGrouping()
        
        self.tabs.removeAll {
            $0.id == id
        }
        
        self.currentTab = nil
    }
}

struct ContentView: View {
    @Environment(\.undoManager) private var manager
    @StateObject private var state: ViewState = ViewState()
    
    @State private var disQuery: String = String()
    
    @State private var presented: Bool = false
    @State private var targeted: Bool = false
    
    private var consoleView: some View {
        ScrollView {
            Text(self.state.consoleText)
                .frame(
                    maxWidth: .infinity,
                    alignment: .leading
                )
                .padding(8)
        }
        .frame(
            minHeight: 100,
            maxHeight: 125
        )
    }
   
    private var tabsView: some View {
        VSplitView {
            TabView(selection: Binding<UUID?>(
                get: { self.state.currentTab },
                set: { self.state.currentTab = $0 }
            )) {
                ForEach(self.state.tabs) { tab in
                    TabContentView(tab: tab)
                        .tag(Optional.some(tab.id))
                        .tabItem { TabLabelView(name: tab.name) }
                }
            }
            .tabViewStyle(.grouped)
            
            self.consoleView
        }
    }

    private var emptyView: some View {
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
    }
    
    private var baseView: some View {
        NavigationSplitView {
            TreeContent(files: self.state.files)
        } detail: {
            self.tabsView
        }
    }

    @ViewBuilder private var workspaceView: some View {
        ZStack {
            if state.tabs.isEmpty {
                self.emptyView
            } else {
                self.baseView
            }
        }
        .frame(
            maxWidth: .infinity,
            maxHeight: .infinity
        )
        .background(
            self.targeted
                ? Color.accentColor.opacity(0.15)
                : Color(nsColor: .textBackgroundColor)
        )
        .overlay {
            RoundedRectangle(cornerRadius: 8)
                .stroke(
                    self.targeted
                        ? Color.accentColor
                        : .clear,
                    lineWidth: 2
                )
                .padding(4)
        }
        .onDrop(
            of: ["public.url"],
            isTargeted: self.$targeted
        ) { providers in

            guard let provider = providers.first else {
                return false
            }

            _ = provider.loadObject(
                ofClass: URL.self
            ) { url, _ in

                guard let url else {
                    return
                }

                DispatchQueue.main.async {
                    self.state.push(path: url)
                }
            }

            return true
        }
        .fileImporter(
            isPresented: self.$presented,
            allowedContentTypes: [.executable]
        ) { result in

            if case .success(let url) = result {
                state.push(path: url)
            }

            self.presented = false
        }
        .onAppear {
            self.state.setManager(manager)
        }
        .onChange(of: manager) { _, manager in
            self.state.setManager(manager)
        }
    }
    
    @ToolbarContentBuilder private var toolbarView: some ToolbarContent {
        ToolbarItemGroup(placement: .navigation) {
            Button {
                presented = true
            } label: {
                Image(systemName: "folder")
            }
        }
        
        ToolbarSpacer(.fixed, placement: .navigation)
        
        ToolbarItemGroup(placement: .navigation) {
            Button {
                manager?.undo()
            } label: {
                Image(systemName: "arrow.uturn.backward")
            }
            .disabled(manager?.canUndo != true)
            .help(manager?.undoActionName ?? "Undo")
            
            Button {
                manager?.redo()
            } label: {
                Image(systemName: "arrow.uturn.forward")
            }
            .disabled(manager?.canRedo != true)
            .help(manager?.redoActionName ?? "Redo")
        }
    }
    
    private var statusbarView: some View {
        HStack {
            Text("Status: \(self.state.status)")
                .foregroundStyle(.secondary)
            
            Spacer()
        }
        .padding(.horizontal, 10)
        .frame(maxWidth: .infinity)
        .frame(height: 24)
        .background(.bar)
    }
    
    public var body: some View {
        VStack(spacing: 0) {
            VSplitView {
                self.workspaceView
            }
           
            self.statusbarView
        }
        .frame(
            maxWidth: .infinity,
            maxHeight: .infinity
        )
        .toolbar {
            self.toolbarView
        }
        .task {
            for await event in self.state.backend.events {
                switch event {
                case .message(let message):
                    self.state.consoleText += AttributedString("\(message)\n")
                }
            }
        }
    }
    
    func push(path: URL) -> Void {
        self.state.push(path: path)
    }
    
    func closeCurrent() -> Void {
        if self.state.currentTab != nil {
            self.state.close(self.state.currentTab!)
        }
    }
}

public struct AboutView: View {
    public var body: some View {
        ScrollView {
            VStack {
                Text("OpenREFW")
                    .font(.largeTitle)
                Text("Version 0.1.0")
                    .font(.footnote)
                
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
    
    @State private var query: String = String()
    @State private var matches: [Int] = []
    @State private var current: Int = 0
    
    private var lines: [AttributedString] = []
    private var plain: [String] = []
    
    var body: some View {
        ScrollViewReader { proxy in
            ScrollView {
                LazyVStack(alignment: .leading, spacing: 0) {
                    ForEach(Array(self.lines.indices), id: \.self) { index in
                        let matches: Bool = self.matches.contains(index)
                       
                        Text(self.lines[index])
                            .fixedSize(horizontal: false, vertical: true)
                            .frame(
                                maxWidth: .infinity,
                                alignment: .leading
                            )
                            .padding(.horizontal, 4)
                            .background {
                                RoundedRectangle(cornerRadius: 4)
                                    .fill(
                                        !self.matches.isEmpty && self.matches[self.current] == index
                                            ? Color.accentColor.opacity(0.25)
                                            : matches ? Color.accentColor.opacity(0.10) : .clear
                                    )
                            }
                            .id(index)
                    }
                }
            }
            .padding(.horizontal)
            .padding(.vertical, 8)
            .searchable(
                text: self.$query,
                placement: .toolbar,
                prompt: "Search in disassembly..."
            )
            .onChange(of: self.query) { _, other in
                let query: String = other.trimmingCharacters(
                    in: .whitespacesAndNewlines
                )
                guard !query.isEmpty else {
                    self.matches = []
                    self.current = 0
                    return
                }
                
                let found = self.plain.indices.filter {
                    self.plain[$0]
                        .localizedCaseInsensitiveContains(query)
                }
                
                self.matches = found
                self.current = 0
                
                if let first = found.first {
                    withAnimation {
                        proxy.scrollTo(first, anchor: .center)
                    }
                }
            }
            .onSubmit(of: .search) {
                guard !self.matches.isEmpty else { return }
                
                self.current = (self.current + 1) % self.matches.count
                
                withAnimation {
                    proxy.scrollTo(
                        self.matches[self.current],
                        anchor: .center
                    )
                }
            }
            .toolbar {
                ToolbarItemGroup(placement: .primaryAction) {
                    if !self.matches.isEmpty {
                        Text("\(self.current + 1) / \(self.matches.count)")
                        
                        Button {
                            self.current = (self.current - 1 + self.matches.count) % self.matches.count
                            
                            withAnimation {
                                proxy.scrollTo(
                                    self.matches[self.current],
                                    anchor: .center
                                )
                            }
                        } label: {
                            Image(systemName: "chevron.up")
                        }
                        
                        Button {
                            self.current = (self.current + 1) % self.matches.count
                            
                            withAnimation {
                                proxy.scrollTo(
                                    self.matches[self.current],
                                    anchor: .center
                                )
                            }
                        } label: {
                            Image(systemName: "chevron.down")
                        }
                    }
                }
            }
        }
    }
    
    public init(text: AttributedString) {
        self.text = text

        var result: [AttributedString] = []

        let nsAttr: NSAttributedString = NSAttributedString(text)
        let nsString: NSString = nsAttr.string as NSString

        nsString.enumerateSubstrings(
            in: NSRange(location: 0, length: nsString.length),
            options: [.byLines]
        ) { _, range, _, _ in
            result.append(
                AttributedString(
                    nsAttr.attributedSubstring(from: range)
                )
            )
        }

        self.lines = result
        self.plain = result.map {
            String(NSAttributedString($0).string)
        }
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
            placement: .sidebar,
            prompt: "Search symbol..."
        )
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
    
    public init(section: Core.Section) {
        self.section = section
        self.labels = section.labels.map { String($0) }
    }
}
