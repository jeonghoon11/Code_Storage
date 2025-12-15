import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.util.ArrayList;
import java.util.concurrent.Callable;
import java.util.function.Consumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TodoListGUI extends JFrame {
    private static final String API_BASE_URL = "http://localhost:3000/todos";
    private static final int CONNECT_TIMEOUT = 5000;
    private static final int READ_TIMEOUT = 5000;
    private static final int WINDOW_WIDTH = 900;
    private static final int WINDOW_HEIGHT = 600;
    
    private static final String STATUS_COMPLETED = "완료";
    private static final String STATUS_PENDING = "미완료";
    private static final String COMPLETED_TRUE = "true";
    private static final String COMPLETED_FALSE = "false";
    
    private static final String ERROR_LOAD_FAILED = "할 일을 불러오는데 실패했습니다: ";
    private static final String ERROR_CREATE_FAILED = "할 일 추가에 실패했습니다: ";
    private static final String ERROR_UPDATE_FAILED = "할 일 수정에 실패했습니다: ";
    private static final String ERROR_DELETE_FAILED = "할 일 삭제에 실패했습니다: ";
    private static final String ERROR_STATUS_CHANGE_FAILED = "상태 변경에 실패했습니다: ";
    private static final String ERROR_SELECT_EDIT = "수정할 할 일을 선택해주세요.";
    private static final String ERROR_SELECT_DELETE = "삭제할 할 일을 선택해주세요.";
    private static final String ERROR_SELECT_TOGGLE = "상태를 변경할 할 일을 선택해주세요.";
    private static final String ERROR_EMPTY_FIELDS = "제목과 내용을 모두 입력해주세요.";
    private static final String CONFIRM_DELETE = "정말 삭제하시겠습니까?";
    private static final String TITLE_DELETE = "삭제 확인";
    private static final String TITLE_ERROR = "오류";
    private static final String TITLE_EDIT = "할 일 수정";
    
    private TodoController todoController;
    private DefaultTableModel tableModel;
    private JTable todoTable;
    private JTextField titleField;
    private JTextArea descriptionArea;
    private ArrayList<String> todoIds;
    
    public TodoListGUI() {
        setTitle("TodoList 관리");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setLocationRelativeTo(null);
        
        todoController = new TodoController();
        todoIds = new ArrayList<>();
        
        initializeComponents();
        layoutComponents();
        loadTodos();
    }
    
    private void initializeComponents() {
        initializeTable();
        initializeInputFields();
    }
    
    private void initializeTable() {
        String[] columnNames = {"제목", "내용", "완료 여부"};
        tableModel = new DefaultTableModel(columnNames, 0) {
            @Override
            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };
        todoTable = new JTable(tableModel);
        todoTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        todoTable.getColumnModel().getColumn(0).setPreferredWidth(200);
        todoTable.getColumnModel().getColumn(1).setPreferredWidth(400);
        todoTable.getColumnModel().getColumn(2).setPreferredWidth(100);
        todoTable.setDefaultRenderer(Object.class, new StrikethroughCellRenderer());
    }
    
    private void initializeInputFields() {
        titleField = new JTextField(20);
        descriptionArea = new JTextArea(5, 20);
        descriptionArea.setLineWrap(true);
        descriptionArea.setWrapStyleWord(true);
    }
    
    private void layoutComponents() {
        setLayout(new BorderLayout());
        add(createInputPanel(), BorderLayout.NORTH);
        add(createTableScrollPane(), BorderLayout.CENTER);
        add(createButtonPanel(), BorderLayout.SOUTH);
    }
    
    private JPanel createInputPanel() {
        JPanel inputPanel = new JPanel(new BorderLayout());
        inputPanel.setBorder(BorderFactory.createTitledBorder("새 할 일 추가"));
        
        JPanel formPanel = createFormPanel();
        JButton addButton = new JButton("추가");
        addButton.addActionListener(new AddTodoListener());
        
        inputPanel.add(formPanel, BorderLayout.CENTER);
        inputPanel.add(addButton, BorderLayout.SOUTH);
        
        return inputPanel;
    }
    
    private JPanel createFormPanel() {
        JPanel formPanel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.anchor = GridBagConstraints.WEST;
        
        addFormField(formPanel, gbc, "제목:", titleField, 0);
        addFormField(formPanel, gbc, "내용:", new JScrollPane(descriptionArea), 1);
        
        return formPanel;
    }
    
    private void addFormField(JPanel panel, GridBagConstraints gbc, String label, Component component, int row) {
        gbc.gridx = 0;
        gbc.gridy = row;
        gbc.fill = GridBagConstraints.NONE;
        gbc.weightx = 0;
        panel.add(new JLabel(label), gbc);
        
        gbc.gridx = 1;
        gbc.fill = (row == 1) ? GridBagConstraints.BOTH : GridBagConstraints.HORIZONTAL;
        gbc.weightx = 1.0;
        gbc.weighty = (row == 1) ? 1.0 : 0;
        panel.add(component, gbc);
    }
    
    private JScrollPane createTableScrollPane() {
        JScrollPane scrollPane = new JScrollPane(todoTable);
        scrollPane.setBorder(BorderFactory.createTitledBorder("할 일 목록"));
        return scrollPane;
    }
    
    private JPanel createButtonPanel() {
        JPanel buttonPanel = new JPanel(new FlowLayout());
        
        buttonPanel.add(createButton("새로고침", e -> loadTodos()));
        buttonPanel.add(createButton("수정", new EditButtonListener()));
        buttonPanel.add(createButton("상태 변경", new ToggleStatusListener()));
        buttonPanel.add(createButton("삭제", new DeleteButtonListener()));
        
        return buttonPanel;
    }
    
    private JButton createButton(String text, ActionListener listener) {
        JButton button = new JButton(text);
        button.addActionListener(listener);
        return button;
    }
    
    private void loadTodos() {
        todoController.loadTodos(
            todos -> SwingUtilities.invokeLater(() -> updateTable(todos)),
            error -> SwingUtilities.invokeLater(() -> showError(error))
        );
    }
    
    private void updateTable(ArrayList<Todo> todos) {
        tableModel.setRowCount(0);
        todoIds.clear();
        
        for (Todo todo : todos) {
            String status = todo.isCompleted() ? STATUS_COMPLETED : STATUS_PENDING;
            todoIds.add(todo.getId());
            tableModel.addRow(new Object[]{todo.getTitle(), todo.getDescription(), status});
        }
    }
    
    private int getSelectedRow() {
        return todoTable.getSelectedRow();
    }
    
    private String getTodoId(int row) {
        return todoIds.get(row);
    }
    
    private String getTodoStatus(int row) {
        return (String) tableModel.getValueAt(row, 2);
    }
    
    private String getTodoTitle(int row) {
        return (String) tableModel.getValueAt(row, 0);
    }
    
    private String getTodoDescription(int row) {
        return (String) tableModel.getValueAt(row, 1);
    }
    
    private class AddTodoListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            String title = titleField.getText().trim();
            String description = descriptionArea.getText().trim();
            
            if (title.isEmpty() || description.isEmpty()) {
                showError(ERROR_EMPTY_FIELDS);
                return;
            }
            
            todoController.createTodo(
                title, description,
                () -> SwingUtilities.invokeLater(() -> {
                    clearInputFields();
                    loadTodos();
                }),
                error -> SwingUtilities.invokeLater(() -> showError(error))
            );
        }
    }
    
    private class EditButtonListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            int row = getSelectedRow();
            if (row == -1) {
                showError(ERROR_SELECT_EDIT);
                return;
            }
            
            String id = getTodoId(row);
            String title = getTodoTitle(row);
            String description = getTodoDescription(row);
            String status = getTodoStatus(row);
            
            new EditDialog(TodoListGUI.this, todoController, id, title, description, status, 
                          () -> loadTodos()).setVisible(true);
        }
    }
    
    private class ToggleStatusListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            int row = getSelectedRow();
            if (row == -1) {
                showError(ERROR_SELECT_TOGGLE);
                return;
            }
            
            String id = getTodoId(row);
            String currentStatus = getTodoStatus(row);
            String newCompleted = currentStatus.equals(STATUS_COMPLETED) 
                ? COMPLETED_FALSE 
                : COMPLETED_TRUE;
            String title = getTodoTitle(row);
            String description = getTodoDescription(row);
            
            todoController.toggleStatus(
                id, title, description, newCompleted,
                () -> SwingUtilities.invokeLater(() -> loadTodos()),
                error -> SwingUtilities.invokeLater(() -> showError(error))
            );
        }
    }
    
    private class DeleteButtonListener implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            int row = getSelectedRow();
            if (row == -1) {
                showError(ERROR_SELECT_DELETE);
                return;
            }
            
            String id = getTodoId(row);
            
            int confirm = JOptionPane.showConfirmDialog(
                TodoListGUI.this,
                CONFIRM_DELETE,
                TITLE_DELETE,
                JOptionPane.YES_NO_OPTION
            );
            
            if (confirm == JOptionPane.YES_OPTION) {
                todoController.deleteTodo(
                    id,
                    () -> SwingUtilities.invokeLater(() -> loadTodos()),
                    error -> SwingUtilities.invokeLater(() -> showError(error))
                );
            }
        }
    }
    
    private void clearInputFields() {
        titleField.setText("");
        descriptionArea.setText("");
    }
    
    private void showError(String message) {
        JOptionPane.showMessageDialog(this, message, TITLE_ERROR, JOptionPane.ERROR_MESSAGE);
    }
    
    public static void main(String[] args) {
        try {
            if (System.getProperty("os.name").toLowerCase().contains("mac")) {
                System.setProperty("apple.laf.useScreenMenuBar", "true");
                System.setProperty("apple.awt.application.name", "TodoList 관리");
            }
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        SwingUtilities.invokeLater(() -> {
            new TodoListGUI().setVisible(true);
        });
    }
    
    private static class Todo {
        private String id;
        private String title;
        private String description;
        private String completed;
        
        public Todo(String id, String title, String description, String completed) {
            this.id = id;
            this.title = title;
            this.description = description;
            this.completed = completed;
        }
        
        public String getId() { return id; }
        public String getTitle() { return title; }
        public String getDescription() { return description; }
        public boolean isCompleted() { return COMPLETED_TRUE.equals(completed); }
    }
    
    private class StrikethroughCellRenderer extends DefaultTableCellRenderer {
        @Override
        public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected,
                                                       boolean hasFocus, int row, int column) {
            Component component = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
            
            Object statusValue = table.getModel().getValueAt(row, 2);
            if (statusValue != null && STATUS_COMPLETED.equals(statusValue.toString())) {
                if (component instanceof JLabel) {
                    JLabel label = (JLabel) component;
                    String text = label.getText();
                    text = text.replaceAll("<[^>]*>", "");
                    label.setText("<html><s>" + text + "</s></html>");
                }
            } else {
                if (component instanceof JLabel) {
                    JLabel label = (JLabel) component;
                    String text = label.getText();
                    text = text.replaceAll("<[^>]*>", "");
                    label.setText(text);
                }
            }
            
            return component;
        }
    }
    
    private static class EditDialog extends JDialog {
        private final TodoController todoController;
        private final String todoId;
        private final String currentCompleted;
        private final Runnable onSuccess;
        
        private JTextField titleField;
        private JTextArea descriptionArea;
        
        public EditDialog(JFrame parent, TodoController todoController, String todoId, 
                         String currentTitle, String currentDescription, String currentCompleted,
                         Runnable onSuccess) {
            super(parent, TITLE_EDIT, true);
            this.todoController = todoController;
            this.todoId = todoId;
            this.currentCompleted = currentCompleted;
            this.onSuccess = onSuccess;
            
            setSize(500, 350);
            setLocationRelativeTo(parent);
            
            initializeComponents(currentTitle, currentDescription);
            layoutComponents();
        }
        
        private void initializeComponents(String currentTitle, String currentDescription) {
            titleField = new JTextField(currentTitle, 30);
            descriptionArea = new JTextArea(currentDescription, 8, 30);
            descriptionArea.setLineWrap(true);
            descriptionArea.setWrapStyleWord(true);
        }
        
        private void layoutComponents() {
            JPanel panel = new JPanel(new BorderLayout());
            panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            
            JPanel formPanel = createFormPanel();
            JPanel buttonPanel = createButtonPanel();
            
            panel.add(formPanel, BorderLayout.CENTER);
            panel.add(buttonPanel, BorderLayout.SOUTH);
            
            add(panel);
        }
        
        private JPanel createFormPanel() {
            JPanel formPanel = new JPanel(new GridBagLayout());
            GridBagConstraints gbc = new GridBagConstraints();
            gbc.insets = new Insets(10, 10, 10, 10);
            gbc.anchor = GridBagConstraints.WEST;
            
            addFormField(formPanel, gbc, "제목:", titleField, 0);
            addFormField(formPanel, gbc, "내용:", new JScrollPane(descriptionArea), 1);
            
            return formPanel;
        }
        
        private void addFormField(JPanel panel, GridBagConstraints gbc, String label, Component component, int row) {
            gbc.gridx = 0;
            gbc.gridy = row;
            gbc.fill = GridBagConstraints.NONE;
            gbc.weightx = 0;
            panel.add(new JLabel(label), gbc);
            
            gbc.gridx = 1;
            gbc.fill = (row == 1) ? GridBagConstraints.BOTH : GridBagConstraints.HORIZONTAL;
            gbc.weightx = 1.0;
            gbc.weighty = (row == 1) ? 1.0 : 0;
            panel.add(component, gbc);
        }
        
        private JPanel createButtonPanel() {
            JPanel buttonPanel = new JPanel(new FlowLayout());
            
            JButton saveButton = new JButton("저장");
            saveButton.addActionListener(e -> handleSave());
            
            JButton cancelButton = new JButton("취소");
            cancelButton.addActionListener(e -> dispose());
            
            buttonPanel.add(saveButton);
            buttonPanel.add(cancelButton);
            
            return buttonPanel;
        }
        
        private void handleSave() {
            String title = titleField.getText().trim();
            String description = descriptionArea.getText().trim();
            
            if (title.isEmpty() || description.isEmpty()) {
                showError(ERROR_EMPTY_FIELDS);
                return;
            }
            
            String completed = currentCompleted.equals(STATUS_COMPLETED) 
                ? COMPLETED_TRUE 
                : COMPLETED_FALSE;
            
            todoController.updateTodo(
                todoId, title, description, completed,
                () -> {
                    dispose();
                    onSuccess.run();
                },
                error -> showError(error)
            );
        }
        
        private void showError(String message) {
            JOptionPane.showMessageDialog(this, message, TITLE_ERROR, JOptionPane.ERROR_MESSAGE);
        }
    }
    
    private static class TodoService {
        public ArrayList<Todo> getAllTodos() throws Exception {
            String response = ApiClient.get("");
            return JsonUtil.parseTodos(response);
        }
        
        public Todo createTodo(String title, String description) throws Exception {
            String json = JsonUtil.createJson(title, description, COMPLETED_FALSE);
            String response = ApiClient.post(json);
            return parseSingleTodo(response);
        }
        
        public Todo updateTodo(String id, String title, String description, String completed) throws Exception {
            String json = JsonUtil.createJson(title, description, completed);
            String response = ApiClient.put(id, json);
            return parseSingleTodo(response);
        }
        
        public void deleteTodo(String id) throws Exception {
            ApiClient.delete(id);
        }
        
        private Todo parseSingleTodo(String json) throws Exception {
            ArrayList<Todo> todos = JsonUtil.parseTodos("[" + json + "]");
            if (todos.isEmpty()) {
                throw new Exception("응답 파싱 실패");
            }
            return todos.get(0);
        }
    }
    
    private static class TodoController {
        private final TodoService todoService;
        
        public TodoController() {
            this.todoService = new TodoService();
        }
        
        public void loadTodos(Consumer<ArrayList<Todo>> onSuccess, Consumer<String> onError) {
            executeAsync(
                () -> todoService.getAllTodos(),
                onSuccess,
                error -> onError.accept(ERROR_LOAD_FAILED + error)
            );
        }
        
        public void createTodo(String title, String description, Runnable onSuccess, Consumer<String> onError) {
            executeAsync(
                () -> todoService.createTodo(title, description),
                result -> onSuccess.run(),
                error -> onError.accept(ERROR_CREATE_FAILED + error)
            );
        }
        
        public void updateTodo(String id, String title, String description, String completed, 
                              Runnable onSuccess, Consumer<String> onError) {
            executeAsync(
                () -> todoService.updateTodo(id, title, description, completed),
                result -> onSuccess.run(),
                error -> onError.accept(ERROR_UPDATE_FAILED + error)
            );
        }
        
        public void toggleStatus(String id, String title, String description, String newCompleted,
                                Runnable onSuccess, Consumer<String> onError) {
            executeAsync(
                () -> todoService.updateTodo(id, title, description, newCompleted),
                result -> onSuccess.run(),
                error -> onError.accept(ERROR_STATUS_CHANGE_FAILED + error)
            );
        }
        
        public void deleteTodo(String id, Runnable onSuccess, Consumer<String> onError) {
            executeAsync(
                () -> {
                    todoService.deleteTodo(id);
                    return null;
                },
                result -> onSuccess.run(),
                error -> onError.accept(ERROR_DELETE_FAILED + error)
            );
        }
        
        private <T> void executeAsync(Callable<T> task, Consumer<T> onSuccess, Consumer<String> onError) {
            new Thread(() -> {
                try {
                    T result = task.call();
                    onSuccess.accept(result);
                } catch (Exception e) {
                    onError.accept(e.getMessage());
                }
            }).start();
        }
    }
    
    private static class ApiClient {
        public static String get(String endpoint) throws Exception {
            HttpURLConnection conn = createConnection(endpoint, "GET");
            return readResponse(conn, HttpURLConnection.HTTP_OK);
        }
        
        public static String post(String json) throws Exception {
            HttpURLConnection conn = createConnection("", "POST");
            conn.setDoOutput(true);
            writeRequestBody(conn, json);
            return readResponse(conn, HttpURLConnection.HTTP_CREATED, HttpURLConnection.HTTP_OK);
        }
        
        public static String put(String id, String json) throws Exception {
            HttpURLConnection conn = createConnection("/" + id, "PUT");
            conn.setDoOutput(true);
            writeRequestBody(conn, json);
            return readResponse(conn, HttpURLConnection.HTTP_OK);
        }
        
        public static void delete(String id) throws Exception {
            HttpURLConnection conn = createConnection("/" + id, "DELETE");
            int responseCode = conn.getResponseCode();
            if (responseCode != HttpURLConnection.HTTP_OK) {
                throw new Exception("HTTP " + responseCode);
            }
        }
        
        private static HttpURLConnection createConnection(String endpoint, String method) throws Exception {
            URL url = new URI(API_BASE_URL + endpoint).toURL();
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod(method);
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setRequestProperty("Accept", "application/json");
            conn.setConnectTimeout(CONNECT_TIMEOUT);
            conn.setReadTimeout(READ_TIMEOUT);
            return conn;
        }
        
        private static void writeRequestBody(HttpURLConnection conn, String json) throws Exception {
            OutputStream os = conn.getOutputStream();
            os.write(json.getBytes("UTF-8"));
            os.flush();
            os.close();
        }
        
        private static String readResponse(HttpURLConnection conn, int... expectedCodes) throws Exception {
            int responseCode = conn.getResponseCode();
            
            if (!contains(expectedCodes, responseCode)) {
                String errorMessage = readErrorStream(conn);
                throw new Exception("HTTP " + responseCode + (errorMessage.isEmpty() ? "" : ": " + errorMessage));
            }
            
            return readInputStream(conn);
        }
        
        private static boolean contains(int[] array, int value) {
            for (int item : array) {
                if (item == value) return true;
            }
            return false;
        }
        
        private static String readInputStream(HttpURLConnection conn) throws Exception {
            try (BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()))) {
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = in.readLine()) != null) {
                    response.append(line);
                }
                return response.toString();
            }
        }
        
        private static String readErrorStream(HttpURLConnection conn) {
            try (BufferedReader in = new BufferedReader(new InputStreamReader(conn.getErrorStream()))) {
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = in.readLine()) != null) {
                    response.append(line);
                }
                return response.toString();
            } catch (Exception e) {
                return "";
            }
        }
    }
    
    private static class JsonUtil {
        private static final Pattern OBJECT_PATTERN = Pattern.compile("\\{[^}]+\\}");
        
        public static ArrayList<Todo> parseTodos(String json) {
            ArrayList<Todo> todos = new ArrayList<>();
            Matcher matcher = OBJECT_PATTERN.matcher(json);
            
            while (matcher.find()) {
                Todo todo = parseTodo(matcher.group());
                if (todo != null) {
                    todos.add(todo);
                }
            }
            
            return todos;
        }
        
        private static Todo parseTodo(String json) {
            try {
                String id = extractValue(json, "_id");
                String title = extractValue(json, "title");
                String description = extractValue(json, "description");
                String completed = extractValue(json, "completed");
                
                return new Todo(id, title, description, completed);
            } catch (Exception e) {
                return null;
            }
        }
        
        private static String extractValue(String json, String key) {
            Pattern pattern = Pattern.compile("\"" + Pattern.quote(key) + "\"\\s*:\\s*\"([^\"]+)\"");
            Matcher matcher = pattern.matcher(json);
            return matcher.find() ? matcher.group(1) : "";
        }
        
        public static String createJson(String title, String description, String completed) {
            return String.format("{\"title\":\"%s\",\"description\":\"%s\",\"completed\":\"%s\"}",
                escapeJson(title), escapeJson(description), completed);
        }
        
        private static String escapeJson(String str) {
            return str.replace("\\", "\\\\")
                      .replace("\"", "\\\"")
                      .replace("\n", "\\n")
                      .replace("\r", "\\r")
                      .replace("\t", "\\t");
        }
    }
}
