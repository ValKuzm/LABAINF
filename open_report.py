import webbrowser
import os
import sys
import time
from datetime import datetime

def open_html_report():
    """Открывает HTML отчет в браузере"""
    
    # Сначала проверяем test_output.txt
    if os.path.exists("test_output.txt"):
        print("=" * 60)
        print("РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ:")
        print("=" * 60)
        with open("test_output.txt", "r", encoding="utf-8") as f:
            content = f.read()
            
            # Выводим только важные части
            if "RESULTS:" in content:
                results_start = content.find("RESULTS:")
                print(content[results_start:])
            else:
                # Показываем последние 20 строк
                lines = content.strip().split('\n')
                for line in lines[-20:]:
                    if line.strip():
                        print(line)
        
        print("\n" + "=" * 60)
    
    # Имена возможных файлов отчетов (в порядке приоритета)
    report_files = [
        "analysis_report.html",
        "test_report.html", 
        "sample_report.html",
        "sample_analysis.html",
        "analysis_full.html"
    ]
    
    print("\nПОИСК ОТЧЕТОВ...")
    print("-" * 40)
    
    found_reports = []
    for report in report_files:
        if os.path.exists(report):
            file_size = os.path.getsize(report) / 1024  # KB
            file_time = datetime.fromtimestamp(os.path.getmtime(report))
            found_reports.append((report, file_size, file_time))
            print(f"? {report:25} {file_size:6.1f} KB  {file_time.strftime('%H:%M:%S')}")
    
    if not found_reports:
        print("? Отчеты не найдены!")
        print("\nСоздайте отчет через программу анализатора.")
        input("\nНажмите Enter для выхода...")
        return
    
    print(f"\nНайдено отчетов: {len(found_reports)}")
    
    # Открываем самый новый отчет
    latest_report = max(found_reports, key=lambda x: x[2])[0]
    file_path = os.path.abspath(latest_report)
    
    print(f"\nОТКРЫВАЮ: {latest_report}")
    print(f"ПУТЬ: {file_path}")
    
    # Открываем в браузере
    try:
        webbrowser.open(f'file://{file_path}')
        print("? Отчет открыт в браузере!")
    except Exception as e:
        print(f"? Ошибка при открытии: {e}")
        print(f"\nОткройте файл вручную: {file_path}")
    
    # Предлагаем выбор отчета
    if len(found_reports) > 1:
        print(f"\nДРУГИЕ ОТЧЕТЫ:")
        for i, (report, size, ftime) in enumerate(found_reports, 1):
            print(f"{i}. {report:20} {size:6.1f} KB")
        
        choice = input(f"\nОткрыть другой отчет (1-{len(found_reports)}) или Enter для выхода: ").strip()
        if choice.isdigit() and 1 <= int(choice) <= len(found_reports):
            selected = found_reports[int(choice)-1][0]
            webbrowser.open(f'file://{os.path.abspath(selected)}')
            print(f"? Открыт: {selected}")
    
    input("\nНажмите Enter для выхода...")

def main():
    print("=" * 60)
    print("    CAESAR CIPHER - ПРОСМОТР РЕЗУЛЬТАТОВ")
    print("=" * 60)
    print()
    
    # Автоматически открываем отчеты
    open_html_report()

if __name__ == "__main__":
    main()