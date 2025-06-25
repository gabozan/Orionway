import numpy as np
from sklearn.linear_model import LinearRegression, RANSACRegressor
import cv2
import itertools
from sklearn.cluster import DBSCAN
import random


def get_intersection(l1, l2):
    rho1, theta1 = l1
    rho2, theta2 = l2
    A = np.array([
        [np.cos(theta1), np.sin(theta1)],
        [np.cos(theta2), np.sin(theta2)]
    ])
    b = np.array([[rho1], [rho2]])
    if np.abs(np.linalg.det(A)) < 1e-6:
        return None
    intersection = np.linalg.solve(A, b)
    return intersection.flatten()

def to_uint8(img):
    norm = (img - np.min(img)) / (np.max(img) - np.min(img))
    return np.round(norm * 255).astype(np.uint8)

def is_empty(x):
    if x is None:
        return True
    try:
        return len(x) == 0
    except TypeError:
        return False
    
def noise_filter(gray: np.ndarray) -> np.ndarray:
    """
    Aplica un filtre de soroll a la imatge en escala de grisos.

    Args:
        gray (np.ndarray): Imatge en escala de grisos.

    Retorna:
        np.ndarray: Imatge filtrada.
    """
    try:
        # kernel_size amb valor 5, ja que aquest ha sigut el valor que millor ha funcionat al projecte ZebrAI Crossing
        return cv2.medianBlur(gray, 5)
    except:
        return gray
    
def find_threshold(src: np.ndarray) -> float:
    """
    Troba el threshold per a una imatge en escala de grisos.

    Args:
        src (np.ndarray): Imatge en escala de grisos.

    Retorna:
        float: Threshold.
    """
    try:
        thr, _ = cv2.threshold(src, 0, 256, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        return thr
    except:
        return 128

def get_edges(bw: np.ndarray) -> np.ndarray:
    """
    Troba els contorns d'una imatge en blanc i negre.

    Args:
        bw (np.ndarray): Imatge en blanc i negre.

    Retorna:
        np.ndarray: Contorns trobats.
    """
    try:
        # Thresholds ajustats per a una millor detecció de contorns amb els valors obtinguts al projecte ZebrAI Crossing
        edges = cv2.Canny(bw, 25, 150, apertureSize=3)
        return edges
    except:
        return np.zeros_like(bw)

def point_line_distance(x0, y0, rho, theta):
    return abs(x0 * np.cos(theta) + y0 * np.sin(theta) - rho)

def get_filtered_lines(edges: np.ndarray) -> list:
    """
    Troba les línies en una imatge utilitzant la transformada de Hough.

    Args:
        edges (np.ndarray): Imatge amb els contorns trobats.

    Retorna:
        list: Línies trobades.
    """
    try:
        few_raw = cv2.HoughLines(edges, 1, np.pi/180, threshold=220)
        raw = cv2.HoughLines(edges, 1, np.pi/180, threshold=140)
        if is_empty(few_raw) or len(few_raw) <= 1:
            raise ValueError("Hough restrictiu no detecta línies")
        
        few_lines = few_raw[:, 0, :]
        lines = raw[:, 0, :]
        intersections = []
        for l1, l2 in itertools.combinations(few_lines, 2):
            pt = get_intersection(l1, l2)
            if pt is not None and np.all(np.isfinite(pt)):
                intersections.append(pt)
        if not intersections:
            return few_lines
        intersections = np.array(intersections)

        clustering = DBSCAN(eps=40, min_samples=5).fit(intersections)
        labels = clustering.labels_
        if np.sum(labels != -1) == 0:
            return few_lines

        unique_labels = np.unique(labels[labels != -1])
        cluster_sizes = np.array([np.sum(labels == k) for k in unique_labels])
        centroids = np.stack([intersections[labels == k].mean(axis=0) for k in unique_labels])
        centroids = centroids[np.argsort(cluster_sizes)[::-1]]

        points = []
        for centroid in centroids:
            if all(centroid[0]-p[0] > 120 and centroid[1]-p[1] > 120 for p in points):
                points.append(centroid)
        points = np.array(points)

        filtered_lines = []
        for rho, theta in lines:
            for x, y in points:
                if point_line_distance(x, y, rho, theta) < 50:
                    filtered_lines.append([rho, theta])
                    break
        filtered_lines = np.array(filtered_lines)

        return filtered_lines
    except:
        return []
    

def remove_duplicate_lines(lines, shape):
    if is_empty(lines):
        raise ValueError("Filtratge de punt de fuga elimina totes les línies")

    lines = np.array(lines)
    h, w = shape[:2]
    d = np.hypot(h, w)
    theta_mean = np.mean(lines[:, 1])
    keep = np.ones(len(lines), dtype=bool)

    def get_line_points(rho, theta):
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a * rho
        y0 = b * rho
        pt1 = np.array([x0 + d * -b, y0 + d * a])
        pt2 = np.array([x0 - d * -b, y0 - d * a])
        return pt1, pt2

    def intersection(p1, p2, q1, q2):
        A = np.array([[p2[0] - p1[0], q1[0] - q2[0]],
                      [p2[1] - p1[1], q1[1] - q2[1]]])
        b = np.array([q1[0] - p1[0], q1[1] - p1[1]])
        if np.linalg.matrix_rank(A) < 2:
            return None
        t = np.linalg.solve(A, b)
        return p1 + t[0] * (p2 - p1)

    for i in range(len(lines)):
        if not keep[i]:
            continue
        rho1, theta1 = lines[i]
        p1, p2 = get_line_points(rho1, theta1)
        for j in range(i + 1, len(lines)):
            if not keep[j]:
                continue
            rho2, theta2 = lines[j]
            q1, q2 = get_line_points(rho2, theta2)
            inter = intersection(p1, p2, q1, q2)
            if inter is not None:
                x, y = inter
                if 0 <= x < w and 0 <= y < h:
                    dist_i = abs(theta1 - theta_mean)
                    dist_j = abs(theta2 - theta_mean)
                    if dist_i > dist_j:
                        keep[i] = False
                        break
                    else:
                        keep[j] = False
    return lines[keep]

def count_near_points(p1, p2, pts, dist):
    pts = np.asarray(pts, dtype=float)
    p1  = np.asarray(p1,  dtype=float)
    p2  = np.asarray(p2,  dtype=float)

    v = p2 - p1
    norm_v = np.linalg.norm(v)

    if norm_v == 0:
        raise ValueError("p1 y p2 no poden coincidir.")
    dif   = p1 - pts
    cross = v[0] * dif[:, 1] - v[1] * dif[:, 0]

    dists = np.abs(cross) / norm_v
    return np.sum(dists < dist)

def get_line_ransac(points,):
    n_lines = len(points)
    best_p=(0,0)
    best_k=0
    if points is not None:
        for i in range(1000):
            p1 = points[random.randint(0, n_lines-1)]
            p2 = points[random.randint(0, n_lines-1)]
            while np.all(p1 == p2):
                p2 = points[random.randint(0, n_lines-1)]
            k = count_near_points(p1,p2, points, 15)
            if k > best_k:
                best_k = k
                best_p = (p1, p2)
    return best_p

def segment_from_rho_theta(rho, theta, edge_img, epsilon=1.0):

    ys, xs = np.nonzero(edge_img) 
    pts = np.stack([xs, ys], axis=1).astype(np.float32)

    n = np.array([np.cos(theta), np.sin(theta)]) 
    v = np.array([-np.sin(theta),  np.cos(theta)])
    d = np.abs(pts @ n - rho)
    on_line = pts[d < epsilon]

    if len(on_line) < 2:
        return None
    
    t = on_line @ v
    p_min = on_line[np.argmin(t)]
    p_max = on_line[np.argmax(t)]
    
    return tuple(p_min.astype(int)), tuple(p_max.astype(int))

def get_limits(representative_lines, edges):
    lista_extremos = []
    for (rho, theta) in representative_lines:
        extremos = segment_from_rho_theta(rho, theta, edges, epsilon=1.5)
        if extremos is not None:
            lista_extremos.append(extremos)
    lista_extremos = np.array(lista_extremos)

    # Retornem els límits de la imatge en cas de no trobar múltiples extrems
    if len(lista_extremos) <= 1:
        h, w = edges.shape
        return ((0,0), (0,h)), ((w,0), (w,h))

    izq = lista_extremos[:, 0]
    der = lista_extremos[:, 1]
    p_izq=get_line_ransac(izq, 15, 1000)
    p_der=get_line_ransac(der, 15, 1000)

    return p_izq, p_der


def intersect_polar_with_segment(rho, theta, segment, eps=1e-6):
    (x1, y1), (x2, y2) = segment
    dx, dy = x2 - x1, y2 - y1
    denom = dx*np.cos(theta) + dy*np.sin(theta)
    if abs(denom) < eps:
        return None
    t = (rho - (x1*np.cos(theta) + y1*np.sin(theta))) / denom
    x = x1 + t*dx
    y = y1 + t*dy
    return (int(round(x)), int(round(y)))

def get_mid_points(lines, p_izq, p_der, shape):

    lista_midpoints = []
    h, w = shape

    for rho, theta in lines:
        sin_t, cos_t = np.sin(theta), np.cos(theta)
        if abs(sin_t) < 1e-6:
            continue
        pi = intersect_polar_with_segment(rho, theta, p_izq)
        pd = intersect_polar_with_segment(rho, theta, p_der)

        # Calcul i filtrat de interseccions
        border = []
        y0 = (rho - 0*cos_t)/sin_t
        yw = (rho - w*cos_t)/sin_t
        if 0 <= y0 <= h: border.append((0, int(round(y0))))
        if 0 <= yw <= h: border.append((w, int(round(yw))))
        x0 = (rho - 0*sin_t)/cos_t if abs(cos_t)>1e-6 else None
        xh = (rho - h*sin_t)/cos_t if abs(cos_t)>1e-6 else None
        if x0 is not None and 0 <= x0 <= w: border.append((int(round(x0)), 0))
        if xh is not None and 0 <= xh <= w: border.append((int(round(xh)), h))
        if len(border) < 2:
            continue
        
        border = sorted(border, key=lambda p: p[0])
        start_border, end_border = border[0], border[-1]

        # Seleccio de punts: interseccions si existeixen, si no, els extrems de la imatge
        start = pi if pi is not None else start_border
        end   = pd if pd is not None else end_border

        # Calcul de punt mitjà
        mx = int(round((start[0] + end[0]) / 2))
        my = int(round((start[1] + end[1]) / 2))
        lista_midpoints.append((mx, my))

    return lista_midpoints

def get_angle(midpoints, shape):
    h, w = shape
    pts = np.array(midpoints)
    xs, ys = pts[:,0], pts[:,1]
    X = xs.reshape(-1,1)
    y = ys

    ransac = RANSACRegressor(LinearRegression(), 
                            residual_threshold = w * 0.005,
                            max_trials=1000, random_state=42)
    ransac.fit(X, y)

    m = 1/ransac.estimator_.coef_[0] if ransac.estimator_.coef_[0] != 0 else np.inf
    angle_rad = np.atan(m)
    
    return angle_rad