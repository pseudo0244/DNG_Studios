# score_editor.py
from PyQt5.QtWidgets import QWidget, QLabel, QVBoxLayout

class ScoreEditor(QWidget):
    def __init__(self, sheet_name):
        super().__init__()
        self.setWindowTitle(f"Score Editor - {sheet_name}")
        self.setGeometry(200, 200, 600, 400)

        layout = QVBoxLayout()

        label = QLabel(f"You are editing: {sheet_name}")
        layout.addWidget(label)

        # You can add more UI elements like a canvas or buttons here

        self.setLayout(layout)
