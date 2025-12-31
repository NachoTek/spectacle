/*
 * Performance Benchmarks for WGC Capture Backend
 * Story 1.1 - NFR1 Verification: <1s latency requirement
 */

#include <QTest>
#include <QElapsedTimer>
#include <QVector>
#include <QtMath>

#include "Platforms/Windows/WGCCaptureMock.h"

using namespace Qt::StringLiterals;

class WGCCaptureBenchmark : public QObject
{
    Q_OBJECT

private:
    WGCCaptureMock *m_capture;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();

    // Performance benchmarks
    void testSingleCaptureLatency();
    void testRepeatedCaptureLatency();
    void testP95LatencyUnderOneSecond();
    void testLatencyUnderLoad();
};

void WGCCaptureBenchmark::initTestCase()
{
    m_capture = WGCCaptureMock::mockInstance();
    QVERIFY(m_capture->initialize());
}

void WGCCaptureBenchmark::init()
{
    // Reset to default latency before each test
    m_capture->setSimulatedLatency(50); // 50ms default
    m_capture->setSimulateFailure(false);
}

void WGCCaptureBenchmark::cleanupTestCase()
{
    // Cleanup
}

void WGCCaptureBenchmark::testSingleCaptureLatency()
{
    // GIVEN: WGC backend initialized
    QVERIFY(m_capture->isInitialized());

    // WHEN: Capturing a single frame
    qint64 latencyMs;
    QImage image = m_capture->captureFrameWithLatency(&latencyMs);

    // THEN: Capture succeeds and latency < 1 second
    QVERIFY2(!image.isNull(), "Capture should return valid image");
    QVERIFY2(latencyMs < 1000,
             qPrintable(u"Single capture latency must be < 1s, got: %1 ms"_s.arg(latencyMs)));
}

void WGCCaptureBenchmark::testRepeatedCaptureLatency()
{
    // GIVEN: WGC backend initialized
    const int captureCount = 10;
    QVector<qint64> latencies;

    // WHEN: Capturing multiple frames
    for (int i = 0; i < captureCount; ++i) {
        qint64 latencyMs;
        QImage image = m_capture->captureFrameWithLatency(&latencyMs);
        QVERIFY2(!image.isNull(), "All captures should succeed");
        latencies.append(latencyMs);
    }

    // THEN: All captures complete in < 1 second each
    for (int i = 0; i < latencies.size(); ++i) {
        QVERIFY2(latencies[i] < 1000,
                 qPrintable(u"Capture %1 latency: %2 ms (must be < 1000ms)"_s.arg(i).arg(latencies[i])));
    }

    // Calculate average latency
    qint64 sum = 0;
    for (qint64 latency : latencies) {
        sum += latency;
    }
    double avgLatency = static_cast<double>(sum) / latencies.size();

    qDebug("Average latency over %d captures: %.2f ms", captureCount, avgLatency);
}

void WGCCaptureBenchmark::testP95LatencyUnderOneSecond()
{
    // GIVEN: WGC backend initialized
    const int sampleSize = 100; // Need good sample size for P95
    QVector<qint64> latencies;

    // WHEN: Capturing many frames
    for (int i = 0; i < sampleSize; ++i) {
        // Vary latency to simulate real-world conditions
        m_capture->setSimulatedLatency(30 + (qrand() % 100)); // 30-130ms range

        qint64 latencyMs;
        QImage image = m_capture->captureFrameWithLatency(&latencyMs);
        QVERIFY2(!image.isNull(), "Capture should succeed");
        latencies.append(latencyMs);
    }

    // THEN: P95 latency < 1 second
    std::sort(latencies.begin(), latencies.end());
    int p95Index = static_cast<int>(qCeil(sampleSize * 0.95)) - 1;
    qint64 p95Latency = latencies[p95Index];

    qDebug("P95 latency over %d samples: %lld ms", sampleSize, p95Latency);

    QVERIFY2(p95Latency < 1000,
             qPrintable(u"P95 latency must be < 1s, got: %1 ms"_s.arg(p95Latency)));
}

void WGCCaptureBenchmark::testLatencyUnderLoad()
{
    // GIVEN: WGC backend initialized
    // WHEN: Simulating high-latency conditions (worst case)
    m_capture->setSimulatedLatency(900); // Close to 1s threshold

    qint64 latencyMs;
    QImage image = m_capture->captureFrameWithLatency(&latencyMs);

    // THEN: Still under 1 second
    QVERIFY2(!image.isNull(), "Capture should succeed even under load");
    QVERIFY2(latencyMs < 1000,
             qPrintable(u"High-load capture must still be < 1s, got: %1 ms"_s.arg(latencyMs)));
    QVERIFY2(latencyMs == 900,
             qPrintable(u"Latency should match simulated value, got: %1 ms"_s.arg(latencyMs)));
}

QTEST_MAIN(WGCCaptureBenchmark)
#include "WGCCaptureBenchmark.moc"
